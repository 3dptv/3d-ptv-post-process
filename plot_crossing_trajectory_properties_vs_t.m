function plot_crossing_trajectory_properties_vs_t(traj)
% Plot trajectory properties in time
% modified for the trajectories crossing the TNTI
%
% Plots three figures: vs time, radius (dot marks the 1st point, if
% it's on the right side, then the particle enters the TNTI from
% non-turbulent side) and for separate \omega components
% 

% Example:
%   load('../../MAT_files/traj_5ppm_freq_0_430_frames_10000_12584_vorticity.mat')
%   load('TrajID_close_to_TNTI.mat')
% % plot for frame no. 10
% plot_crossing_trajectory_properties_vs_t(Traj(TrajID_close_to_TNTI{10}))
%
%


% Author: Alex Liberzon
% Copyright (c) 2015 Turbulence Structure Laboratory, TAU
% Last modified: April 5, 2015

% 

if isempty(traj), disp('No trajectories to plot'); return; end

x0 = -39.13; %[mm]
y0 = 50;%[mm]
z0 = 0;%[mm]
fps = 100;



figure,

%%
quantities = {'omegax','omegay','omegaz'};
displaynames = {'\omega_x','\omega_y','\omega_z'};
lines = {'r-','g--','b-.'};

subplot(311)
box on, grid on, hold on,
for i = 1:length(traj)
    traj1(i).t = traj(i).t - traj(i).t(1);
    
    for j = 1:length(quantities)
        plot(traj1(i).t/fps,traj(i).(quantities{j}),lines{j});
    end
end
hold off
xlabel('t [sec]','Interpreter','latex');
ylabel('x [mm]','Interpreter','latex');
legend(displaynames)
%

%%
subplot(312)
box on, grid on, hold on,

quantities = {'uf','vf','wf'};
displaynames = {'u','v','w'};

for i = 1:length(traj)
    traj1(i).t = traj(i).t - traj(i).t(1);
    
    for j = 1:length(quantities)
        plot(traj1(i).t/fps,traj(i).(quantities{j}),lines{j});
    end
end
hold off
xlabel('t [sec]','Interpreter','latex');
ylabel('x [mm]','Interpreter','latex');
legend(displaynames)

%%
subplot(313)
box on, grid on, hold on,

quantities = {'axf','ayf','azf'};
displaynames = {'a_x','a_y','a_z'};

for i = 1:length(traj)
    traj1(i).t = traj(i).t - traj(i).t(1);
    
    for j = 1:length(quantities)
        plot(traj1(i).t/fps,traj(i).(quantities{j}),lines{j});
    end
end
hold off
xlabel('t [sec]','Interpreter','latex');
ylabel('x [mm]','Interpreter','latex');
legend(displaynames)




%% Now let's plot versus radius - if r is decreasing - it crosses in one
%% way, if grows, in another


figure

%%
quantities = {'omegax','omegay','omegaz'};
displaynames = {'$\omega^2$'};

subplot(311)
box on, grid on, hold on,
for i = 1:length(traj)
    r = sqrt((traj(i).xf-x0).^2 + (traj(i).yf-y0).^2 + (traj(i).zf-z0).^2);
    tmp = traj(i).xf*0;
    for j = 1:length(quantities)
        tmp = tmp + traj(i).(quantities{j}).^2 ;
    end
    plot(r,tmp);
    plot(r(1),tmp(1),'o')
end
hold off
xlabel('r [mm]','Interpreter','latex');
ylabel(displaynames,'Interpreter','latex');
% legend(displaynames)
set(gca,'yscale','log')


%%
quantities = {'uf','vf','wf'};
displaynames = {'$TKE$'};

subplot(312)
box on, grid on, hold on,
for i = 1:length(traj)
    r = sqrt((traj(i).xf-x0).^2 + (traj(i).yf-y0).^2 + (traj(i).zf-z0).^2);
    tmp = traj(i).xf*0;
    for j = 1:length(quantities)
        tmp = tmp + traj(i).(quantities{j}).^2 ;
    end
    plot(r,tmp);
    plot(r(1),tmp(1),'o')
end
hold off
xlabel('r [mm]','Interpreter','latex');
ylabel(displaynames,'Interpreter','latex');
set(gca,'yscale','log')
% legend(displaynames)

%%
quantities = {'axf','ayf','azf'};
displaynames = {'$a^2$'};

subplot(313)
box on, grid on, hold on,
for i = 1:length(traj)
    r = sqrt((traj(i).xf-x0).^2 + (traj(i).yf-y0).^2 + (traj(i).zf-z0).^2);
    tmp = traj(i).xf*0;
    for j = 1:length(quantities)
        tmp = tmp + traj(i).(quantities{j}).^2 ;
    end
    plot(r,tmp);
    plot(r(1),tmp(1),'o')
end
hold off
xlabel('r [mm]','Interpreter','latex');
ylabel(displaynames,'Interpreter','latex');
set(gca,'yscale','log')
% legend(displaynames)



%%
figure

%%
quantities = {'omegax','omegay','omegaz'};
displaynames = {'\omega_x','\omega_y','\omega_z'};
lines = {'r-','g--','b-.'};

% subplot(311)
box on, grid on, hold on,
for i = 1:length(traj)
    r = sqrt((traj(i).xf-x0).^2 + (traj(i).yf-y0).^2 + (traj(i).zf-z0).^2);
    for j = 1:length(quantities)
        tmp = traj(i).(quantities{j});
        plot(r,tmp.^2,lines{j},'DisplayName',displaynames{j});
        plot(r(1), tmp(1)^2,'o');
    end
end
hold off
xlabel('r [mm]','Interpreter','latex');
% ylabel(displaynames,'Interpreter','latex');
% legend(displaynames)
% legend(gca,'show')
set(gca,'yscale','log')
