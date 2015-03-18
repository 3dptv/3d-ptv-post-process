function [traj] = readTrajPoint_scanning(varargin)
% 
%
% Example:
% 
% traj = readTrajPoint_scanning('../../test_data/',101000,101025,3);
%

nCol = 33; % for scanning
% see
% https://github.com/3dptv/3d-ptv-post-process/blob/scanning_Oct2014/scanning_Oct2014/list_of_output_columns.jpg
 

traj = struct(...
    'xf',[],'yf',[],'zf',[],...
    'uf',[],'vf',[],'wf',[],...
    'axf',[],'ayf',[],'azf',[],...
    'omegax',[],'omegay',[],'omegaz',[],...
    's11',[],'s12',[],'s13',[],...
    's22',[],'s23',[],'s33',[],...
    'ut',[],'vt',[],'wt',[],...
    'daxdx',[],'daxdy',[],'daxdz',[],...
    'daydx',[],'daydy',[],'daydz',[],...
    'dazdx',[],'dazdy',[],'dazdz',[],...
    'trajid',0,'t',[],'particle_id',0);

fieldNames = fieldnames(traj);

if nargin ~= 4 % no inputs
    
    [filename1, pathname] = uigetfile('trajPoint.*', 'Pick a FIRST trajectory file');
    
    if isequal(filename1,0) || isequal(pathname,0)
        error('Wrong selection')
    end
    
    wd = cd;
    cd(pathname);
    
    [filename2, pathname] = uigetfile('trajPoint.*', 'Pick a LAST trajectory file');
    if isequal(filename2,0) || isequal(pathname,0)
        cd(wd); error('Wrong selection')
    end
    
    
    % First and last index of the files
    firstFileIndx =  eval(filename1(findstr(filename1,'.') + 1:end));
    lastFileIndx =  eval(filename2(findstr(filename2,'.') + 1:end));
    
    cd(wd);
    
    MINLENGTH = 3;

    
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
    fid = fopen(sprintf('%s%s%s%d',pathname,filesep,'trajPoint.',n));
    if fid == -1, error('no file'), continue, end
    f = fscanf(fid,'%g',[nCol inf]); % It has two rows now.
    if isempty(f), continue , end; % if the file is empty, skip to next file
    f = f(1:33,:)';
    fclose(fid);
    
    
    
    % prepare indices of the single trajectories
    [startTraj, endTraj, lenTraj] = singleTraj(f(:,32));
    
    % pick only long trajectories, longer than ' MINLENGTH '
    longTraj = find(lenTraj >= MINLENGTH);
    if isempty(longTraj), fprintf(1,'!'), continue, end; % if there are no long trajectories, move to the next file
    
    
    numLongTraj = length(longTraj);     % number of long trajectories
    startTraj = startTraj(longTraj);    % starting
    endTraj = endTraj(longTraj);        % ending points
    lenTraj = lenTraj(longTraj);        % lengthes
    %     maxLenTraj = max(lenTraj);          % maximum length
    
    f(:,1:3) = 1000*f(:,1:3); % meters to mm
    
    for i = 1:numLongTraj
        
        
        for q = 1:30 
            traj(trajIndx).(fieldNames{q})(1:lenTraj(i)) = ...
                f(startTraj(i):endTraj(i),q)';
        end

        traj(trajIndx).t(1:lenTraj(i)) = ...
            f(startTraj(i):endTraj(i),32)'+runIndx;
        
        traj(trajIndx).trajid(1:lenTraj(i)) = repmat(trajIndx,1,lenTraj(i)).';
        
        % last column is the id of the particle in ptv_is files
        traj(trajIndx).(fieldNames{33})(1:lenTraj(i)) = ...
                f(startTraj(i):endTraj(i),33)';
        
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