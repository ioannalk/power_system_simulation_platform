%%
% Automatic microgrid analysis and solution script 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%% System simulation parameters' evaluation %%%%

handle = getSimulinkBlockHandle(strcat(gcs,'/powergui'));
if ~strcmp(get_param(handle,'SolverType'),'Tustin')
    set_param(handle,'SolverType','Tustin'); % Trapezoidal Rule for numerical solution is selected
end
if ~strcmp(get_param(handle,'x0status'),'zero')
    set_param(handle,'x0status','zero'); % Zero Initial State
end
sps = power_analyze(gcs,'sort'); % Derive schematic information
Dt = sps.SampleTime; % Simulation Step
Start = str2num(get_param(gcs,'StartTime'));
Stop = str2num(get_param(gcs,'StopTime'));
time = Start:Dt:Stop;
count = 0;

%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
max_f1 = 0;
max_f2 = 0;
i_max_f1 = 0;
i_max_f2 = 0;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

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
    [V_n, I_br, i_max_f1, i_max_f2] = initial_state_fp(time(1), rlc, src, No_Brn, No_Nodes, No_Src, i_max_f1, i_max_f2);
    Out = outp_assign(Out, outputs, No_Out, V_n, I_br, 1);
    count = 2;
end

%%
%%%%%%%%%%%%%%%%%%% Simulation %%%%%%%%%%%%%%%%%%%

while count <= size(time,2)
    % New values of sources
    for i=1:No_Src
        if src(i,6) ~= 0 % AC source
            src(i,8) = src(i,4)*sin(2*pi*src(i,6)*time(count) + src(i,5)*pi/180);
        else % DC source
            src(i,8) = src(i,4);
        end
    end
    Vk = Vk_assign(No_Vk, No_Src, idx, src); % Create known voltages vector, Vk
    [I_hs, max_f1, max_f2] = Ihs_assign(No_Brn, rlc, I_hs, V_n, I_br, max_f1, max_f2);
    [I_history, max_f1, max_f2] = I_history_assign(No_Nodes, No_Brn, No_Src, rlc, src, I_hs, max_f1, max_f2);
    
    if No_Vk ~= 0 % Voltage sources exist
        I_history = I_history_correction(I_history, Vk_nodes, No_Vk, No_Nodes);
        I_U = I_history(1:(No_Nodes-No_Vk),1); 
        [V_U, max_f1, max_f2] = Vnodal_fp(I_history, I_U, G, G_UU, G_UK, Vk, 1, max_f1, max_f2);
        % Assign nodal voltages in the correct order
        c = 0;
        for i=1:No_Nodes
            if any(find(Vk_nodes == i))
                index = find(Vk_nodes == i);
                V_n(i) = Vk(index);
            else
                c = c + 1;
                V_n(i) = V_U(c);
            end
        end
    else % Only current sources exist
        [V_n, max_f1, max_f2] = Vnodal_fp(I_history, [], G, [], [], 0, 0, max_f1, max_f2);
    end
    [I_br, max_f1, max_f2] = Ibr_assign(No_Brn, rlc, I_hs, V_n, G_br, I_br, max_f1, max_f2);
    
    Out = outp_assign(Out, outputs, No_Out, V_n, I_br, count);
    count = count + 1;
end

%%
%%%%%%%%%%%%%%%% Fixed-Point Analysis %%%%%%%%%%%%%%%%

fm = get_fimath();
w = 32;

% Include initial state (Comment to exclude)
max_f1 = max(max_f1, i_max_f1);
max_f2 = max(max_f2, i_max_f2);

% Convert to fixed-point 
max_f1 = fi(max_f1, 1, w, fm);
max_f2 = fi(max_f2, 1, w, fm);

% Determine fraction lengths
f1 = max_f1.FractionLength;
f2 = max_f2.FractionLength;

% Clear variables from Workspace
clear ('i_max_f1', 'i_max_f2', 'max_f1', 'max_f2', 'w', 'fm');

%%
%%%%%%%%%%%%%%%% Error Estimation between Simulink and NIS method %%%%%%%%%%%%%%%%

prompt = {"Do you want to compute the error between Simulink and NIS method? Answer 'y' or 'n'"};
answer = inputdlg(prompt, "Input", [1 35]);

if answer{1,1} == 'y' && strcmp(gcs,'Sphase')
    Verr = abs(Out(1, :) - logsout{1}.Values.Data(:, 1)');
    Ierr = abs(Out(2, :) - logsout{2}.Values.Data(:, 1)');
    plot_error(Verr, Ierr, time, sps);

elseif answer{1,1} == 'y' && strcmp(gcs,'Tphase')
    Verr_A = abs(Out(1, :) - logsout{1}.Values.Data(:, 1)');
    Verr_B = abs(Out(2, :) - logsout{1}.Values.Data(:, 2)');
    Verr_C = abs(Out(3, :) - logsout{1}.Values.Data(:, 3)');
    Ierr_A = abs(Out(4, :) - logsout{2}.Values.Data(:, 1)');
    Ierr_B = abs(Out(5, :) - logsout{2}.Values.Data(:, 2)');
    Ierr_C = abs(Out(6, :) - logsout{2}.Values.Data(:, 3)');
    Verr = [Verr_A; Verr_B; Verr_C];
    Ierr = [Ierr_A; Ierr_B; Ierr_C];
    plot_error(Verr, Ierr, time, sps);
    
end