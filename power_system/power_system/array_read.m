function [A] = array_read(name, dim1, dim2, wl, fl)

    A = zeros(dim1, dim2);
    
    for i = 1:dim1
        file = strcat(name, '_', num2str(i), '.txt');
        fileID = fopen(file, 'r');
        
        for j = 1:dim2
            v = fscanf(fileID,'%d',1);
            if v > 2^(wl-1)-1
                v = v - 2^(wl);
            end
            A(i,j) = v*(2^(-fl));
        end
        
        fclose(fileID);
    end
    
end