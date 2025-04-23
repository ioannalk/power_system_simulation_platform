function [Verr, Ierr] = simulation_error(Vnodal, Ibranch, logsout, outputs)

    if strcmp(gcs,'Sphase')
        
        if outputs{1,1}(1) == 0
            Verr = abs( - Vnodal(outputs{1,1}(2),:) - logsout{1}.Values.Data(:,1)');
        elseif outputs{1,1}(2) == 0
            Verr = abs(Vnodal(outputs{1,1}(1),:) - logsout{1}.Values.Data(:,1)');
        else
            Verr = abs((Vnodal(outputs{1,1}(1),:) - Vnodal(outputs{1,1}(2),:)) - logsout{1}.Values.Data(:,1)');
        end
        
        Ierr = abs(Ibranch(outputs{2,1},:) - logsout{2}.Values.Data(:,1)');
  
    elseif strcmp(gcs,'Tphase')
        
        if outputs{1,1}(1) == 0
            Verr_A = abs( - Vnodal(outputs{1,1}(2),:) - logsout{1}.Values.Data(:,1)');
        elseif outputs{1,1}(2) == 0
            Verr_A = abs(Vnodal(outputs{1,1}(1),:) - logsout{1}.Values.Data(:,1)');
        else
            Verr_A = abs((Vnodal(outputs{1,1}(1),:) - Vnodal(outputs{1,1}(2),:)) - logsout{1}.Values.Data(:,1)');
        end
        
        if outputs{2,1}(1) == 0
            Verr_B = abs( - Vnodal(outputs{2,1}(2),:) - logsout{1}.Values.Data(:,2)');
        elseif outputs{2,1}(2) == 0
            Verr_B = abs(Vnodal(outputs{2,1}(1),:) - logsout{1}.Values.Data(:,2)');
        else
            Verr_B = abs((Vnodal(outputs{2,1}(1),:) - Vnodal(outputs{2,1}(2),:)) - logsout{1}.Values.Data(:,2)');
        end
        
        if outputs{3,1}(1) == 0
            Verr_C = abs( - Vnodal(outputs{3,1}(2),:) - logsout{1}.Values.Data(:,3)');
        elseif outputs{3,1}(2) == 0
            Verr_C = abs(Vnodal(outputs{3,1}(1),:) - logsout{1}.Values.Data(:,3)');
        else
            Verr_C = abs((Vnodal(outputs{3,1}(1),:) - Vnodal(outputs{3,1}(2),:)) - logsout{1}.Values.Data(:,3)');
        end
        
        Ierr_A = abs(Ibranch(outputs{4,1},:) - logsout{2}.Values.Data(:,1)');
        Ierr_B = abs(Ibranch(outputs{5,1},:) - logsout{2}.Values.Data(:,2)');
        Ierr_C = abs(Ibranch(outputs{6,1},:) - logsout{2}.Values.Data(:,3)');
            
        Verr = [Verr_A; Verr_B; Verr_C];
        Ierr = [Ierr_A; Ierr_B; Ierr_C];
 
    end
    
end

