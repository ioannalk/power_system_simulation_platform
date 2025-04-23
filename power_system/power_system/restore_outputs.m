function [outputs] = restore_outputs(Outputs_Info, sps)

    outputs_name = extractAfter(string(sps.OutputNames), ': ');
    
    if strcmp(gcs,'Sphase')
        voltage_name = outputs_name{1};
        current_name = outputs_name{2};
        
        for i = 1 : size(Outputs_Info.Names, 1)
            
            full_name = string(Outputs_Info.Names{i,1});
            if contains(full_name, ': ')
                tmp_name = extractAfter(full_name, ': ');
            else
                tmp_name = full_name;
            end
            
            if strcmp(voltage_name, tmp_name)
                voltage_output = i;
                break;
            end
        end
        outputs{1,1}(1) = Outputs_Info.Nodes(voltage_output, 1);
        outputs{1,1}(2) = Outputs_Info.Nodes(voltage_output, 2);
        
        for i = 1 : size(Outputs_Info.Names, 1)
                        
            full_name = string(Outputs_Info.Names{i,1});
            if contains(full_name, ': ')
                tmp_name = extractAfter(full_name, ': ');
            else
                tmp_name = full_name;
            end
            
            if strcmp(current_name, tmp_name)
                current_output = i;
                break;
            end
        end
        outputs{2,1}(1) = current_output;
  
    elseif strcmp(gcs,'Tphase')
        
        voltage_name = outputs_name{1};
        current_name = outputs_name{4};
        
        for i = 1 : size(Outputs_Info.Names, 1)
                        
            full_name = string(Outputs_Info.Names{i,1});
            if contains(full_name, ': ')
                tmp_name = extractAfter(full_name, ': ');
            else
                tmp_name = full_name;
            end
            
            if strcmp(voltage_name, tmp_name)
                voltage_output = i;
                break;
            end
        end
        outputs{1,1}(1) = Outputs_Info.Nodes(voltage_output, 1);
        outputs{1,1}(2) = Outputs_Info.Nodes(voltage_output, 2);
        outputs{2,1}(1) = Outputs_Info.Nodes(voltage_output + 1, 1);
        outputs{2,1}(2) = Outputs_Info.Nodes(voltage_output + 1, 2);
        outputs{3,1}(1) = Outputs_Info.Nodes(voltage_output + 2, 1);
        outputs{3,1}(2) = Outputs_Info.Nodes(voltage_output + 2, 2);
        
        for i = 1 : size(Outputs_Info.Names, 1)
                        
            full_name = string(Outputs_Info.Names{i,1});
            if contains(full_name, ': ')
                tmp_name = extractAfter(full_name, ': ');
            else
                tmp_name = full_name;
            end
            
            if strcmp(current_name, tmp_name)
                current_output = i;
                break;
            end
        end
        outputs{4,1}(1) = current_output;
        outputs{5,1}(1) = current_output + 1;
        outputs{6,1}(1) = current_output + 2; 
    end
    
end

