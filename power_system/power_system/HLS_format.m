%%
% Automatic microgrid analysis and solution script 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%% System simulation parameters' evaluation %%%%

handle = getSimulinkBlockHandle(strcat(gcs,'/powergui'));
if ~strcmp(get_param(handle,'SolverType'),'Tustin')
    set_param(handle,'SolverType','Tustin');
end
if ~strcmp(get_param(handle,'x0status'),'zero')
    set_param(handle,'x0status','zero');
end
sps = power_analyze(gcs,'sort');
Dt = sps.SampleTime;
Start = str2num(get_param(gcs,'StartTime'));
Stop = str2num(get_param(gcs,'StopTime'));
time = Start:Dt:Stop;
count = 0;

%%
%%%%%%%%%%%%%% Power Plant Analysis %%%%%%%%%%%%%%

[rlc,src,outputs,G,G_br,No_Nodes,No_Brn,No_Src,No_Out,I_hs] = PPA(sps, Dt);
V_n = zeros(No_Nodes,1); % Nodal voltages
I_br = zeros(No_Brn,1); % Branch currents
Out = zeros(No_Out,size(time,2)); % Network Analysis simulation results for Error Estimation
[No_Vk, Vk_nodes, idx, G] = G_correction(src, G, No_Nodes, No_Src); % Correction of G matrix if voltage sources exist
src = [src, zeros(No_Src,1)];
if No_Vk ~= 0 % Voltage sources exist, G matrix is fragmented to Guu, Guk, Gku, Gkk
    G_UU = G(1:(No_Nodes-No_Vk),1:(No_Nodes-No_Vk)); 
    G_UU = inverse(G_UU, 1e-9);
    G_UK = G(1:(No_Nodes-No_Vk),(No_Nodes-No_Vk+1):No_Nodes);
else % Only current sources exist
    G = inverse(G, 1e-9);
end
for i=1:No_Src
    if src(i,6) == 0 % DC Source
        count = 1;
        break
    end
end
if count ~= 1 % AC sources only
    [V_n, I_br] = initial_state(time(1), rlc, src, No_Brn, No_Nodes, No_Src);
    Out = outp_assign(Out, outputs, No_Out, V_n, I_br, 1);
    count = 2;
end

%%
%%%%%%%%%%%%%%%%%%%%%  Prompt  %%%%%%%%%%%%%%%%%%%%%%

prompt = {"Enter fraction length f1:","Enter fraction length f2:","Text Files Generation for HLS (1) Simulink/HLS Simulation Error (2) Simulink/ARM Simulation Error (3)"};
answer = inputdlg(prompt, "Input", [1 35]);
fl_f1 = str2num(answer{1,1});
fl_f2 = str2num(answer{2,1});
choice = str2num(answer{3,1});
wl = 32;


%%
if choice == 1
    %%%%%%%%%%%% Text Files Generation for HLS %%%%%%%%%%%%

    fileID = fopen('coeffs.txt', 'w'); % Text File = "coeffs.txt"
    if count == 1 
        time_f = size(time, 2); 
    else
        time_f = size(time, 2) - 1; 
    end
    fprintf(fileID, '%d\r\n%d\r\n%d\r\n', wl, fl_f1, fl_f2);
    fprintf(fileID, '%d\r\n%d\r\n', No_Nodes, No_Brn);
    fprintf(fileID,'%d\r\n%d\r\n', No_Src, No_Vk);
    fprintf(fileID, '%d\r\n', time_f);
    fclose(fileID);

    BrnInfo = [rlc(:,1:2), zeros(No_Brn,3)];
    Pr_Type = -1;
    for i=1:No_Brn
        Type = rlc(i,3);
        if Type == 2
            if Pr_Type ~= 2
                % Primary winding
                U1 = rlc(i,6);
            else
                % Secondary winding
                U2 = rlc(i,6);
                a_inv = U2/U1; % Transformer's inverse turns ratio
                BrnInfo(i,3) = a_inv;
                BrnInfo(i,4) = I_hs(i,3)*a_inv;
                BrnInfo(i,5) = G_br(i)*a_inv;
                Pr_Type = Type;
                continue;
            end
        end
        Pr_Type = Type;
    end
    array_print('BrnInfo',BrnInfo(:,1:2),wl,0);
    array_print('Transf',BrnInfo(:,3:5),wl,fl_f2);
	
    IsInfo = zeros(No_Src-No_Vk,2);
    I_s = zeros(No_Src-No_Vk,size(time,2));
    j = 0;
    for i=1:No_Src
        if src(i,3) == 1 % Current source
            j = j + 1;
            IsInfo(j,:) = src(i,1:2);
            I_s(j,:) = src(i,4)*sin(2*pi*src(i,6).*time + src(i,5)*pi/180);
        end
    end
        if count == 2 
            I_s = I_s(:,2:end);
    end

    VsInfo = zeros(No_Vk,2);
    V_s = zeros(No_Vk,size(time,2));
    j = 0;
    for i=1:No_Src
        if src(i,3) == 0 % Voltage source
            j = j + 1;
            VsInfo(j,:) = src(i,1:2);
            V_s(j,:) = src(i,4)*sin(2*pi*src(i,6).*time + src(i,5)*pi/180);
        end
    end
    if count == 2 
        V_s = V_s(:,2:end);
    end

    if No_Src-No_Vk ~= 0 % Current sources exist
        array_print('IsInfo',IsInfo,wl,0);
        array_print('I_s',I_s,wl,fl_f2);
    end
    if No_Vk ~= 0 % Voltage sources exist
        array_print('VsInfo',VsInfo,wl,0);
        array_print('V_s',V_s,wl,fl_f1);
    end

    if No_Vk ~= 0 % Voltage sources exist
        array_print('G_UU',G_UU,wl,fl_f1);
        array_print('G_UK',G_UK,wl,fl_f2);
    else % Only current sources exist
        array_print('G',G,wl,fl_f1);
    end
    array_print('G_br',G_br,wl,fl_f2);

    fileID = fopen('I_hs.txt', 'w');
    
    I_hs_f2 = fi(I_hs(:,3),1,wl,fl_f2);
    I_hs_f1 = fi(I_hs(:,4),1,wl,fl_f1);
    for i = 1:size(I_hs, 1)
        fprintf(fileID,'%d\r\n%d\r\n',int(I_hs_f2(i)), int(I_hs_f1(i)));
    end
    
    fclose(fileID);
    clear('I_hs_f1', 'I_hs_f2');
    
    array_print('V_n',V_n,wl,fl_f1);
    V_branch = zeros(No_Brn,1);
    for i=1:No_Brn
        if BrnInfo(i,1) ~= 0 && BrnInfo(i,2) ~= 0
            V_branch(i,1) = V_n(BrnInfo(i,1)) - V_n(BrnInfo(i,2));
        elseif BrnInfo(i,1) == 0
            V_branch(i,1) = - V_n(BrnInfo(i,2));
        else
            V_branch(i,1) = V_n(BrnInfo(i,1));
        end
    end
    array_print('V_branch',V_branch,wl,fl_f1);

    array_print('I_br',I_br,wl,fl_f2);

    % Save nodes and branches assignment 
    Outputs_Info.Names = sps.RlcBranchNames;
    Outputs_Info.Nodes(:,1) = sps.RlcBranch(:,1);
    Outputs_Info.Nodes(:,2) = sps.RlcBranch(:,2);
    save('Outputs_Info.mat', 'Outputs_Info');

end


%%
%%%%% Simulink/HLS Simulation Error or Simulink/Real-Time Simulation Error %%%%%

if choice == 2

    % Restore nodes and branches assignment 
    load('Outputs_Info.mat');
    outputs = restore_outputs(Outputs_Info, sps);

    % Read results and plot the error
    HLS_Vnodal = array_read('HLS_V\V',No_Nodes,size(time,2),wl,fl_f1);
    HLS_Ibranch = array_read('HLS_I\I',No_Brn,size(time,2),wl,fl_f2);
    [Verr, Ierr] = simulation_error(HLS_Vnodal, HLS_Ibranch, logsout, outputs);
    plot_error(Verr, Ierr, time, sps);

elseif choice == 3

    % Restore nodes and branches assignment 
    load('Outputs_Info.mat');
    outputs = restore_outputs(Outputs_Info, sps);

    % Read results and plot the error
    FPGA_Vnodal = array_read('FPGA_V\V',No_Nodes,size(time,2),wl,fl_f1);
    FPGA_Ibranch = array_read('FPGA_I\I',No_Brn,size(time,2),wl,fl_f2);
    [Verr, Ierr] = simulation_error(FPGA_Vnodal, FPGA_Ibranch, logsout, outputs);
    plot_error(Verr, Ierr, time, sps);

end