function [I_history, max_f1, max_f2] = I_history_assign(No_Nodes,No_Brn, No_Src, rlc, src, I_hs, max_f1, max_f2)

    % The current vector, which is the sum of history current sources and
    % external current sources, is computed in this function

    max_a = 0;
    max_is = 0;
    
    I_history = zeros(No_Nodes,1);
    Pr_Type = -1;
    U1 = 0;
    for i=1:No_Brn 
        if ~any(I_hs(i,3:end))
            continue
        end
        Type = rlc(i,3);
        Node_P = I_hs(i,1);
        Node_N = I_hs(i,2);
        Brn_I_History = I_hs(i,5);
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        I_history = fun1Ihistory(I_history,Brn_I_History,Node_P,Node_N);
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        if Type == 2 % Transformer branches - no magnetization branches
            if Pr_Type ~= 2
                % Primary winding
                U1 = rlc(i,6);
            else
                % Secondary winding
                U2 = rlc(i,6);
                a = U1/U2;
                % Primary winding nodes
                Node_P_tp = rlc(i+1,1); 
                Node_N_tp = rlc(i+1,2);
                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                I_history = fun2Ihistory(I_history,Brn_I_History,1/a,Node_P_tp,Node_N_tp);
                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

                max_a = max(max_a, abs(1/a));
                max_f2 = max(max_f2, max_a);
                 
            end
        end
        Pr_Type = Type;
    end

    max_ihistory = max(abs(I_history));
    max_f2 = max(max_f2, max_ihistory);

    for i=1:No_Src
        if src(i,3) == 1 % Current source
            Is = src(i,8);
            Node_P = src(i,1);
            Node_N = src(i,2);
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            I_history = fun1Ihistory(I_history,Is,Node_P,Node_N);
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

            max_is = max(max_is, abs(Is));
            max_f2 = max(max_f2, max_is);
        
        end
    end

    max_ihistory = max(abs(I_history));
    max_f2 = max(max_f2, max_ihistory);
    
end

