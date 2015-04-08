function varargout = plot_long_trajectories(traj,minLength,figureNum)
% Plots long trajectories
% Usage:
% PLOT_LONG_TRAJECTORIES(TRAJ,<MINLENGTH>)
% where SCENE5 is a structure that includes
% field 'traj' and optional input MINLENGTH is the
% shortest length of the trajectory to plot.
%
% SEE ALSO: building_trajectories.m

% Author: Alex Liberzon
% Copyright (c) 2007, TAU
% all the xf,yf,zf converted to x,y,z by Jimmy Kim

if nargin > 2
    figure1 = figureNum;
    color = 'r';
else
    % Create figure
    figure1 = figure;
    color = 'b';
    % Create axes
    % axes('Parent',figure1,...
    %     'PlotBoxAspectRatio',[1 1 1],...
    %     'FontSize',12,...
    %     'DataAspectRatio',[1 1.6 1.6],...
    %     'Color',[0.9412 0.9412 0.9412],...
    %     'CameraViewAngle',10.34,...
    %     'CameraUpVector',[0 1 0],...
    %     'CameraPosition',[0.2148 -0.2713 0.4102]);
    axes('Parent',figure1);
    box('on');
    grid('on');
    hold('all');
end



% Create xlabel
xlabel('$x$','Interpreter','Latex');

% Create ylabel
ylabel('$z$','Interpreter','Latex');

% Create zlabel
zlabel('$y$','Interpreter','Latex');


if nargin < 2, minLength = 10; end

trajLen = zeros(length(traj),1);
only_long = isfield(traj,'trajid');
idx = 1:length(traj);
switch only_long
    case 1
        for i = 1:length(traj)
            trajLen(i) = length(traj(i).trajid);
            if traj(i).trajid == -999
                trajLen(i) = 0;
            end
        end
        id = find(trajLen >= minLength);
    case 0
        id = 1:length(traj);
end

if nargin < 3
    for i = 1:length(id)
        %     plot3(traj(id(i)).xf/1000,traj(id(i)).yf/1000,traj(id(i)).zf/1000,...
        plot3(traj(id(i)).xf,traj(id(i)).zf,traj(id(i)).yf,...
            'MarkerFaceColor','w','Marker','o',...
            'MarkerSize',6,...
            'UserData',id(i));
        plot3(traj(id(i)).xf(1),traj(id(i)).zf(1),traj(id(i)).yf(1),...
            'MarkerFaceColor','w','Marker','>',...
            'MarkerSize',8);
    end
else
    for i = 1:length(id)
        plot3(traj(id(i)).xf,traj(id(i)).zf,traj(id(i)).yf,...
            ... 'MarkerFaceColor','w','Marker','x',...
            'Marker','x',...
            'MarkerSize',3,...
            'UserData',id(i));
        %     h = plot3(traj(id(i)).xf,traj(id(i)).zf,traj(id(i)).yf,'--');
        % set(h,'UserData',idx(id(i)));
        % plot3(traj(id(i)).xf,traj(id(i)).yf,traj(id(i)).zf,'x','MarkerSize',4);
        % plot3(traj(id(i)).xf(1),traj(id(i)).zf(1),traj(id(i)).yf(1),'r.','MarkerSize',3);
    end
end

% campos([-0.2208   -0.2416    0.6716]);
% camup([-0.1478    0.9178    0.4739]);
% camtarget([-0.0119    0.0014    0.0014]);

if nargout
    varargout{1} = trajLen;
    varargout{2} = figure1;
end
view(3)
