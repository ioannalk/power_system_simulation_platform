function [I_hs, max_f1, max_f2] = Ihs_assign(No_Brn, rlc, I_hs, V_n, I_br, max_f1, max_f2)
    
% This function computes the history current sources of the system

    max_coeftr = 0;
    max_vbranch = 0;

    Pr_Type = -1;
    U1 = 0;
    for i=1:No_Brn
        if ~any(I_hs(i,3:end))
            continue
        end
        Type = rlc(i,3);
        Node_P = I_hs(i,1);
        Node_N = I_hs(i,2);
        coef1 = I_hs(i,3);
        coef2 = I_hs(i,4);
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
                I_hs(i,5) = fun1Ihs(I_br(i),V_n,coef1,coef2,coef1/a,Node_P,Node_N,Node_P_tp,Node_N_tp);
                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

                max_coeftr = max(max_coeftr, abs(coef1/a));
                max_f2 = max(max_f2, max_coeftr);

                if Node_P ~= 0 && Node_N ~= 0
                    max_vbranch = max(max_vbranch, max(abs(V_n(Node_P) - V_n(Node_N))));
                    max_f1 = max(max_f1, max_vbranch);
                end
                 
                Pr_Type = Type;
                continue
            end
        end
        Pr_Type = Type;
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        I_hs(i,5) = fun2Ihs(V_n,I_br(i),coef1,coef2,Node_P,Node_N);
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        if Node_P ~= 0 && Node_N ~= 0
            max_vbranch = max(max_vbranch, max(abs(V_n(Node_P) - V_n(Node_N))));
            max_f1 = max(max_f1, max_vbranch);
        end
    
    end

    max_coef1 = max(abs(I_hs(:,3)));
    max_f2 = max(max_f2, max_coef1);
    max_coef2 = max(abs(I_hs(:,4)));
    max_f1 = max(max_f1, max_coef2);
    max_ih = max(abs(I_hs(:,5)));
    max_f2 = max(max_f2, max_ih);
    max_ibr = max(abs(I_br));
    max_f2 = max(max_f2, max_ibr);
    max_vn = max(abs(V_n));
    max_f1 = max(max_f1, max_vn);
    
end

        