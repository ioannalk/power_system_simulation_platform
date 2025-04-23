function [] = plot_error(Verr, Ierr, time, sps)

    output_name = extractAfter(string(sps.OutputNames), ': ');

    if strcmp(gcs,'Sphase')
    
        figure;
        plot(time, Verr, '-k');
        title(output_name{1}, 'FontSize', 10);
        legend('Verr'); 
        xlabel('Time (Seconds)');
        
        figure;
        plot(time, Ierr, '-k');
        title(output_name{2}, 'FontSize', 10);
        legend('Ierr'); 
        xlabel('Time (Seconds)');
    
    elseif strcmp(gcs,'Tphase')
    
        figure;
        plot(time, Verr(1,:), '-k', time, Verr(2,:), ':k', time, Verr(3,:), '-.k');
        title(output_name{1}, 'FontSize', 10);
        legend('Verr phase A', 'Verr phase B', 'Verr phase C'); 
        xlabel('Time (Seconds)');
        
        figure;
        plot(time, Ierr(1,:), '-k', time, Ierr(2,:), ':k', time, Ierr(3,:), '-.k');
        title(output_name{4}, 'FontSize', 10);
        legend('Ierr phase A', 'Ierr phase B', 'Ierr phase C'); 
        xlabel('Time (Seconds)');
    
    end

end

