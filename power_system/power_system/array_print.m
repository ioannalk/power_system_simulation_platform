function [] = array_print(name, A, wl, fl)

    file = strcat(name, '.txt');
    fileID = fopen(file, 'w');
    g = fi(A, 1, wl, fl);
    for i = 1:size(g, 1)
        for j = 1:size(g, 2)
            fprintf(fileID, '%d\r\n', int(g(i, j)));
        end
    end
    fclose(fileID);

end

