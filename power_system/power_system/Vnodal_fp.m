function [A, max_f1, max_f2] = Vnodal_fp(I_history,I_U,G,G_UU,G_UK,Vk,flag, max_f1, max_f2)
    
    if flag == 1 % Known voltage sources exist

        I_d_history = I_U - G_UK * Vk; 
        
        max_idhistory = max(abs(I_d_history));
        max_f2 = max(max_f2, max_idhistory);
        max_guu = max(max(abs(G_UU)));
        max_f1 = max(max_f1, max_guu);
        max_guk = max(max(abs(G_UK)));
        max_f2 = max(max_f2, max_guk);
        max_vk = max(abs(Vk));
        max_f1 = max(max_f1, max_vk);
    
    else % Only current sources exist
              
        max_ihistory = max(abs(I_history));
        max_f2 = max(max_f2, max_ihistory);
        max_g = max(max(abs(G)));
        max_f1 = max(max_f1, max_g);
        
    end
    
    A = Vnodal(I_history,I_U,G,G_UU,G_UK,Vk,flag);
    
    max_A = max(abs(A));
    max_f1 = max(max_f1, max_A);
        
end

