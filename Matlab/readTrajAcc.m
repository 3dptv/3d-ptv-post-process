function [traj] = readTrajAcc(varargin)
% TRAJ = READTRAJACC(DIRECTORY,FIRST_ID,LAST_ID,MINTRAJLENGTH)
% reads the trajAcc. files from the DIRECTORY from FIRST_ID to LAST_ID
% and stores only trajectories longer than MINTRAJLENGTH
% If less then 4 arguments have been provided, then the function asks for
% the first and last files and uses default MINTRAJLENGTH = 10 frame steps.
%
% Example
% 
% traj = readTrajAcc('../datasets/april26_trajAcc_Beat',10000,16035,10);
%

traj = struct('x',[],'y',[],'z',[],...
    'u',[],'v',[],'w',[],...
    ...'ux',[],'uy',[],'uz',[],...
    ...'vx',[],'vy',[],'vz',[],...
    ...'wx',[],'wy',[],'wz',[],...
    ...'dudt',[],'dvdt',[],'dwdt',[],...
    ...'ax',[],'ay',[],'az',[],...
    ...'daxdx',[],'daxdy',[],'daxdz',[],...
    ...'daydx',[],'daydy',[],'daydz',[],...
    ...'dazdx',[],'dazdy',[],'dazdz',[],...
    ...'trajnum',0,...
    't',[]);

fieldNames = fieldnames(traj);

if nargin ~= 4 % no inputs
    
    [filename1, pathname] = uigetfile('trajAcc.*', 'Pick a FIRST trajectory file');
    
    if isequal(filename1,0) || isequal(pathname,0)
        error('Wrong selection')
    end
    
    wd = cd;
    cd(pathname);
    
    [filename2, pathname] = uigetfile('trajAcc.*', 'Pick a LAST trajectory file');
    if isequal(filename2,0) || isequal(pathname,0)
        cd(wd); error('Wrong selection')
    end
    
    
    % First and last index of the files
    firstFileIndx =  eval(filename1(findstr(filename1,'.') + 1:end));
    lastFileIndx =  eval(filename2(findstr(filename2,'.') + 1:end));
    
    cd(wd);
    
    MINLENGTH = 10;

    
else
    pathname = varargin{1};
    firstFileIndx = varargin{2};
    lastFileIndx = varargin{3};
    MINLENGTH = varargin{4};
end

fprintf(1,'Wait please ...  \n ')
disp('');
trajIndx = 1;

% Loop through all files
for n = firstFileIndx:lastFileIndx
    
    runIndx = n - firstFileIndx + 1; % running counter
    
    fprintf(1,'\b\b\b\b\b\b\b\b\b');
    fprintf(1,'%9d',n);
    
    % f = load(sprintf('%s%d',baseName,n),'ASCII');
    fid = fopen(sprintf('%s%s%s%d',pathname,filesep,'trajAcc.',n));
    if fid == -1, error('no file'), continue, end
    % f = fscanf(fid,'%g',[35 inf]); % It has two rows now.
    f = fscanf(fid,'%g',[34 inf]); % It has two rows now.
    if isempty(f), continue , end; % if the file is empty, skip to next file
    f = f(1:34,:)';
    fclose(fid);
    
    
    
    % prepare indices of the single trajectories
    [startTraj, endTraj, lenTraj] = singleTraj(f(:,34));
    
    % pick only long trajectories, longer than ' MINLENGTH '
    longTraj = find(lenTraj >= MINLENGTH);
    if isempty(longTraj), fprintf(1,'!'), continue, end; % if there are no long trajectories, move to the next file
    
    
    numLongTraj = length(longTraj);     % number of long trajectories
    startTraj = startTraj(longTraj);    % starting
    endTraj = endTraj(longTraj);        % ending points
    lenTraj = lenTraj(longTraj);        % lengthes
    %     maxLenTraj = max(lenTraj);          % maximum length
    
    for i = 1:numLongTraj
        
        for q = 1:3 % only first 3, x,y,z
            traj(trajIndx).(fieldNames{q})(1:lenTraj(i)) = ...
                1000 * f(startTraj(i):endTraj(i),q)'; % 1000 for mm
        end
        
        for q = 4:6 % only u,v,w
            traj(trajIndx).(fieldNames{q})(1:lenTraj(i)) = ...
                f(startTraj(i):endTraj(i),q)';
        end
%         % trajIndex
%         traj(trajIndx).(fieldNames{13})(1:lenTraj(i)) = ...
%             f(startTraj(i):endTraj(i),31)';
        % running time
        traj(trajIndx).(fieldNames{7})(1:lenTraj(i)) = ...
            f(startTraj(i):endTraj(i),34)'+runIndx;
        trajIndx = trajIndx + 1;
    end
    
end % FILES
end % of function

% ---------------------------------------------------------------------------------------------------------
function [startTraj, endTraj, lenTraj] = singleTraj(age)
% Split the file into single trajectories, according to the
% AGE vector, column 29 of the trajPoint.* files
startTraj = find(age == 0);      % start points of the trajectories
endTraj = startTraj(2:end)-1;    % end points of the trajectories
endTraj = [endTraj;length(age)];
lenTraj = endTraj - startTraj + 1;
%    numTraj = length(startTraj);
end % of function