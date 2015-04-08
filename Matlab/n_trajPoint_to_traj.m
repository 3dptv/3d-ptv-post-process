function traj = n_trajPoint_to_traj(varargin)
% the help is obsolete, to be replaced
% Date modified: March 18, 2014
% Author: @alexlib
%
%
% TRAJACC2HDF Reads trajAcc.* files (32 columns) from latest version of
% Beat Luethi PostPTV(tm) software into Matlab and saves them
% with the HDF format.
%
%    TRAJACC2HDF_v32(HFILE,FILEPATH,FIRSTFILE,LASTFILE)
% Will read trajACC.* files from trajAcc.FIRSTFILE to
% trajAcc.LASTFILE in the directory FILEPATH
%
% Example:
%           trajAcc2HDF('Conv_run90.hdf')
% or
%
%           trajAcc2HDF_V32('conv_run90.hdf','e:\ptv\convection\14july2004\res\',10000,12000);
%
% See also: HELP TRAJACC2HDF_V47 READTRAJHDF HDF HDFTOOL
%
%

% Created: 19-Jul-2004
% Author: Alex Liberzon
% E-Mail : liberzon@ihw.baug.ethz.ch
% Phone : +41 (0)1 63 34004
% Copyright (c) 2004 IHW, ETH, Zuerich
%
% Modified at: 19-Jul-2004
% $Revision: 1.0 $  $Date: 19-Jul-2004 10:10:49$


wd = cd;
baseName = 'n_trajPoint.';
MINLENGTH = 0;

% quantities = varargin{1};       % 'vel', or 'der', or 'all'


traj = struct('xf',[],'yf',[],'zf',[],...
    'uf',[],'vf',[],'wf',[],...
    'dudx',[],'dudy',[],'dudz',[],...
    'dvdx',[],'dvdy',[],'dvdz',[],...
    'dwdx',[],'dwdy',[],'dwdz',[],...
    'dudt',[],'dvdt',[],'dwdt',[],...
    'axf',[],'ayf',[],'azf',[],...
    'daxdx',[],'daxdy',[],'daxdz',[],...
    'daydx',[],'daydy',[],'daydz',[],...
    'dazdx',[],'dazdy',[],'dazdz',[],...
    'trajnum',0,'t',[]);

flds = fieldnames(traj); % for later automatic assignment

fieldname_list = 'x,y,z,u,v,w,dudx,dudy,dudz,dvdx,dvdy,dvdz,dwdx,dwdy,dwdz,dudt,dvdt,dwdt,ax,ay,az,daxdx,daxdy,daxdz,daydx,daydy,daydz,dazdx,dazdy,dazdz,trajnum,t';

units = {'m','m','m',...                % x,y,z
    'm/s','m/s','m/s',...           % u,v,w
    'm/s^2','m/s^2','m/s^2',...     % ax,ay,az
    '1/s','1/s','1/s',...           % w1,w2,w3
    '1/s','1/s','1/s',...           % s11,...
    '1/s','1/s','1/s',...           % s22,...
    '1','1','1',...                 % wW1,...
    '1/s^3','1/s^3','1','1',...     % wws,sss,R,Q
    'm^2/s^3','1','1','1',...       % dissipation, divergence, trajNum, t,
    'm/s^2','m/s^2','m/s^2'};         % alx,aly,alz
% vclass = {'position','position','position',...                % x,y,z
%         'velocity','velocity','velocity',...           % u,v,w
%         'acceleration','acceleration','acceleration',...     % ax,ay,az
%         'vorticity','vorticity','vorticity',...           % w1,w2,w3
%         'strain','strain','strain',...           % s11,...
%         'strain','strain','strain',...           % s22,...
%         'stretching','stretching','stretching',...                 % wW1,...
%         'wws','sss','R','Q',...     % wws,sss,R,Q
%         'dissipation','divergence','trajectory','time',...       % dissipation, divergence, t0, ii,
%         'acceleration','acceleration','acceleration'};         % alx,aly,alz

% dtype = repmat({'double'},32,1);
% dtype{28} = 'int16';
% dtype{29} = 'int16';

if nargin == 0 % no inputs at all 
    [filename1, pathname] = uigetfile('trajAcc.*', 'Pick a FIRST trajectory file');
    
    if isequal(filename1,0) || isequal(pathname,0)
        cd(wd); error('Wrong selection')
    end
    
    cd(pathname)
    [filename2, pathname] = uigetfile('trajAcc.*', 'Pick a LAST trajectory file');
    if isequal(filename2,0) || isequal(pathname,0)
        cd(wd); error('Wrong selection')
    end
    
    % First and last index of the files
    firstFileIndx =  eval(filename1(findstr(filename1,'.')+1:end));
    lastFileIndx =  eval(filename2(findstr(filename2,'.')+1:end));
    
elseif nargin == 4  % expect directory, first file, last file, minlength
    pathname = varargin{1};
    firstFileIndx = varargin{2};
    lastFileIndx =  varargin{3};
    MINLENGTH = varargin{4};
else
    error('Wrong number of inputs');
end



fprintf(1,'Wait please ...  ')
disp('');
trajIndx = 1;
%



% Loop through all files
for n = firstFileIndx:lastFileIndx
    
    runIndx = n - firstFileIndx + 1; % running counter
    
    fprintf(1,'\b\b\b\b\b\b\b\b\b');
    fprintf(1,'%9d',n);
    
    % f = load(sprintf('%s%d',baseName,n),'ASCII');
    fid = fopen(sprintf('%s%s%s%d',pathname,filesep,baseName,n));
    if fid == -1, continue, end
    % f = fscanf(fid,'%g',[35 inf]); % It has two rows now.
    f = fscanf(fid,'%g',[32 inf]); % It has two rows now.
    if isempty(f), continue , end; % if the file is empty, skip to next file
    f = f(1:32,:)';
    fclose(fid);
    
    
    f(:,1:3) = f(:,1:3)*1000; % m to mm
    
    
    [startTraj, endTraj, lenTraj] = singleTraj(f(:,32)); % prepare indices of the single trajectories
    
    % pick only long trajectories, longer than 'len'
    longTraj = find(lenTraj >= MINLENGTH);
    if isempty(longTraj), fprintf(1,'!'), continue, end; % if there are no long trajectories, move to the next file
    
    
    numLongTraj = length(longTraj);     % number of long trajectories
    startTraj = startTraj(longTraj);    % starting
    endTraj = endTraj(longTraj);        % ending points
    lenTraj = lenTraj(longTraj);        % lengthes
    %     maxLenTraj = max(lenTraj);          % maximum length
    
    for i = 1:numLongTraj
        ti = startTraj(i):endTraj(i); % temporary index
        validIndx(startTraj(i):endTraj(i)) = 1;
        for k = 1:length(flds)-2 % assign 30 columns to the respective fields
            traj(trajIndx).(flds{k}) = f(ti,k);
        end
        traj(trajIndx).trajnum = repmat(trajIndx,length(ti),1);
        traj(trajIndx).t = runIndx + f(ti,32);
        trajIndx = trajIndx + 1; % running counter of trajectories

    end
    
   
    
end % FILES
% ---------------------------------------------------------------------------------------------------------


cd(wd);
