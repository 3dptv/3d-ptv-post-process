function varargout = trajAcc2HDF4(hfile,varargin)
% TRAJACC2HDF4 Reads trajAcc.* files (32 columns) from latest version of
% Beat Luethi PostPTV(tm) software into Matlab and saves them 
% with the HDF4 format.
%
% TRAJACC2HDF4(HFILE,FILEPATH,FIRSTFILE,LASTFILE)
% will read trajACC.* files from trajAcc.FIRSTFILE to
% trajAcc.LASTFILE in the directory FILEPATH
%
% Example:
%           trajAcc2HDF4('Conv_run90.hdf')
% or
%
%           trajAcc2HDF4('conv_run90.hdf','e:\ptv\convection\14july2004\res\',10000,12000);
%
% See also: HELP TRAJACC2HDF_V47 
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

%{
fprintf(fpp, "%lf\t", xp[ii]);//1
                     fprintf(fpp, "%lf\t", yp[ii]);//2
                     fprintf(fpp, "%lf\t", zp[ii]);//3
                     fprintf(fpp, "%lf\t", up[ii]);//4
                     fprintf(fpp, "%lf\t", vp[ii]);//5
                     fprintf(fpp, "%lf\t", wp[ii]);//6
                     fprintf(fpp, "%lf\t", axp[ii]);//7
                     fprintf(fpp, "%lf\t", ayp[ii]);//8
                     fprintf(fpp, "%lf\t", azp[ii]);//9
                     fprintf(fpp, "%lf\t", w1p[ii]);//10
                     fprintf(fpp, "%lf\t", w2p[ii]);//11
                     fprintf(fpp, "%lf\t", w3p[ii]);//12
                     fprintf(fpp, "%lf\t", s11p[ii]);//13
                     fprintf(fpp, "%lf\t", s12p[ii]);//14
                     fprintf(fpp, "%lf\t", s13p[ii]);//15
                     fprintf(fpp, "%lf\t", s22p[ii]);//16
                     fprintf(fpp, "%lf\t", s23p[ii]);//17
                     fprintf(fpp, "%lf\t", s33p[ii]);//18
                     fprintf(fpp, "%lf\t", utp[ii]);//19
                     fprintf(fpp, "%lf\t", vtp[ii]);//20
                     fprintf(fpp, "%lf\t", wtp[ii]);//21
                     fprintf(fpp, "%lf\t", daxdxp[ii]);//22
                     fprintf(fpp, "%lf\t", daxdyp[ii]);//23
                     fprintf(fpp, "%lf\t", daxdzp[ii]);//24
                     fprintf(fpp, "%lf\t", daydxp[ii]);//25
                     fprintf(fpp, "%lf\t", daydyp[ii]);//26
                     fprintf(fpp, "%lf\t", daydzp[ii]);//27
                     fprintf(fpp, "%lf\t", dazdxp[ii]);//28
                     fprintf(fpp, "%lf\t", dazdyp[ii]);//29
                     fprintf(fpp, "%lf\t", dazdzp[ii]);//30
                     fprintf(fpp, "%lf\t", quality);//31 0=good, 1=bad
                     fprintf(fpp, "%lf\n", (double)(ii));//32 age along trajectory
%}

error(nargchk(1,5,nargin))


% ---------------------------------------------------------------------------------------------------------        

wd = cd;
baseName = 'g_trajPoint.';
MINLENGTH = 0;

% quantities = varargin{1};       % 'vel', or 'der', or 'all'


traj = struct('x',[],'y',[],'z',[],...
    'u',[],'v',[],'w',[],...
    'ax',[],'ay',[],'az',[],...
    'w1',[], 'w2',[],'w3',[],...
    's11',[],'s12',[],'s13',[],...
    's22',[],'s23',[],'s33',[],...
    'dudt',[],'dvdt',[],'dwdt',[],... 
    'daxdx',[],'daxdy',[],'daxdz',[],...
    'daydx',[],'daydy',[],'daydz',[],...
    'dazdx',[],'dazdy',[],'dazdz',[],...
    'trajnum',0,'t',[]);



fieldname_list = 'x,y,z,u,v,w,ax,ay,az,w1,w2,w3,s11,s12,s13,s22,s23,s33,dudt,dvdt,dwdt,daxdx,daxdy,daxdz,daydx,daydy,daydz,dazdx,dazdy,dazdz,trajnum,t';

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

dtype = repmat({'double'},32,1);
% dtype{28} = 'int16';
% dtype{29} = 'int16';



% ---------- HDF part --------------
try
    % Check if file exists:
    if hdf('H','ishdf', hfile)
        access = 'write';
        %     answer = input('Do you want to overwrite ... (y/n)  ','s');
        %     if strcmp(answer,'y')
        %       hdfml('closeall'), delete(hfile), access = 'create';
        %       feval('TrajPoint2HDF_v7',hfile,varargin{:});
        %     else
        error('File exists') % this option should be changed, if somebody wants to append 28.11.03
        % end
    else
        access = 'create';
    end
    
    % open new or existing hfile
    file_id = hdf('H','open', hfile, access, 0);
    if file_id == -1
        error('HDF hopen failed');
    end
    
    % initialize the V interface
    status = hdf('V','start',file_id);
    if status == -1
        error('HDF vstart failed');
    end
    
    
    % initialize the SD interface
    sd_id = hdf('SD','start', hfile, 'write');
    if sd_id == -1
        error('HDF sdstart failed');
    end
    
    % assign optional top level SD attributes
    astr.Date       =  date;
    astr.Author     = 'Alex Liberzon';
    astr.Comment    = sprintf('By trajAcc2HDF, ver.%d ', 1);
    astr.Directory  = cd;
    
    afields = fieldnames(astr);
    for j = 1 : length(afields)
        status = hdf('SD','setattr', sd_id, afields{j}, astr.(afields{j}));
        if status == -1
            error('HDF sdsetattr failed')
        end
    end
    
    % ---------------------------------------------------------------------------------------------------------        
    
    if nargin < 4
        
        [filename1, pathname] = uigetfile('g_trajPoint.*', 'Pick a FIRST trajectory file');
        
        if isequal(filename1,0) | isequal(pathname,0)
            cd(wd); error('Wrong selection')
        end
        
        cd(pathname)
        [filename2, pathname] = uigetfile('g_trajPoint.*', 'Pick a LAST trajectory file');
        if isequal(filename2,0) | isequal(pathname,0)
            cd(wd); error('Wrong selection')
        end
        
        % First and last index of the files
        firstFileIndx =  eval(filename1(findstr(filename1,'.')+1:end));
        lastFileIndx =  eval(filename2(findstr(filename2,'.')+1:end));
        
    else  % given HDF name, directory, first file, last file
        pathname = varargin{1};
        firstFileIndx = varargin{2};
        lastFileIndx =  varargin{3};
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
        
        
        
        
        [startTraj, endTraj, lenTraj] = singleTraj(f(:,32)); % prepare indices of the single trajectories
        
        % pick only long trajectories, longer than 'len'
        longTraj = find(lenTraj >= MINLENGTH);
        if isempty(longTraj), fprintf(1,'!'), continue, end; % if there are no long trajectories, move to the next file
        
        
        numLongTraj = length(longTraj);     % number of long trajectories
        startTraj = startTraj(longTraj);    % starting  
        endTraj = endTraj(longTraj);        % ending points
        lenTraj = lenTraj(longTraj);        % lengthes
        %     maxLenTraj = max(lenTraj);          % maximum length
        
        validIndx = zeros(size(f,1),1);
        for i = 1:numLongTraj
            validIndx(startTraj(i):endTraj(i)) = 1;
            f(startTraj(i):endTraj(i),31) = trajIndx;
            trajIndx = trajIndx + 1;
        end
        validIndx = find(validIndx);
        
        
        
        
        % ------------- HDF part -------------------
        % create a new vs data 
        access = 'w';
        vdata_ref = -1;                                         % flag to create
        vdata_id = hdf('VS','attach', file_id, vdata_ref, access);
        %         vdata_id = hdf('V',s('attach', file_id, vdata_ref, access);
        if vdata_id == -1
            error('HDF vdata attach failed')
        end
        
        %  offset = 0;
        
        
        % assign the group the name "i"
        % sname = sprintf('%d', lastTrajIndx);
        sname = sprintf('%d', runIndx);
        status = hdf('VS','setname', vdata_id, sname);
        if status == -1
            error('HDF vsetname failed')
        end
        
        % assign the group the class "Trajectory"
        sclass = 'Trajectory';
        status = hdf('VS','setclass', vdata_id, sclass);
        if status == -1
            error('HDF vsetclass failed')
        end
        
        dfields = fieldnames(traj);
        
        for j = 1 : length(dfields)                 % 32 fields
            status = hdf('VS','fdefine',vdata_id, dfields{j},'double',1);
        end
        
        status = hdf('VS','setfields',vdata_id,fieldname_list);
        if status == -1
            error('HDF VS setfields failed')
        end
        
        % Interlace mode is set to full, meaning that data is written row
        % by row of the record, rather than by field (not all x, then all
        % y, but all x,y,z, ... for the record 1, then for 2, etc)
        status = hdf('VS','setinterlace',vdata_id,'full');
        if status == -1
            error('HDF VS setinterlace failed')
        end
        
        f(validIndx,1:3) = 1000 * f(validIndx,1:3);
        
        % write the buffered data into the first vdata with full interlace mode
        num_of_records = hdf('VS','write',vdata_id,{f(validIndx,1)',f(validIndx,2)', f(validIndx,3)', ...
                f(validIndx,4)',f(validIndx,5)', f(validIndx,6)', ...
                f(validIndx,7)',f(validIndx,8)', f(validIndx,9)', ...
                f(validIndx,10)',f(validIndx,11)', f(validIndx,12)', ...
                f(validIndx,13)',f(validIndx,14)', f(validIndx,15)', ...
                f(validIndx,16)',f(validIndx,17)', f(validIndx,18)', ...
                f(validIndx,19)',f(validIndx,20)', f(validIndx,21)', ...
                f(validIndx,22)',f(validIndx,23)', f(validIndx,24)', ...
                f(validIndx,25)',f(validIndx,26)', f(validIndx,27)', ...
                f(validIndx,28)',f(validIndx,29)', f(validIndx,30)', ...
                f(validIndx,31)',f(validIndx,32)'+runIndx});
        
        % nbytes = hdf('VS','sizeof',vdata_id,fieldname_list)
        
        % offset = offset + nbytes*num_of_records % *128;
        
        %     status = hdf('VS','setattr',vdata_id,'vdata','MaxLength',max(lenTraj));
        %     if status == -1
        %         error('HDF vsetattr(1) failed')
        %     end
        %     status = hdf('VS','setattr',vdata_id,'vdata','MaxLength',max(lenTraj));
        %     if status == -1
        %         error('HDF vsetattr(1) failed')
        %     end
        %     status = hdfvs('setattr', vdata_id, 27 , 'lengths' , lenTraj );
        %     if status == -1
        %         error('HDF vsetattr(2) failed')
        %     end
        
        % assign the group the class "Trajectory"
        %     sclass = 'Trajectory';
        sclass = sprintf('%d',max(lenTraj));
        status = hdf('VS','setclass', vdata_id, sclass);
        if status == -1
            error('HDF vsetclass failed')
        end
        
        
        % %     status = hdf('VS','setexternalfile',vdata_id,sprintf('%s.%d',hfile,runIndx), 0);
        % %     if status == -1
        % %         error('HDF v external failed')
        % %     end
        
        
        % detach from the first vdata
        status = hdf('VS','detach',vdata_id);
        if status == -1, error('HDF vsdetach failed'), end
        %         lastTrajIndx = lastTrajIndx + 1; 
        %     end  % of all trajectories in the file
        
    end % FILES
    % ---------------------------------------------------------------------------------------------------------        
    
    
    % end vgroup interface access
    status = hdf('V','end',file_id);
    if status == -1
        error('HDF v end failed')
    end
    
    % end vgroup interface access
    status = hdf('SD','end',sd_id); 
    if status == -1
        error('HDF sd end failed')
    end
    
    % close the HDF file
    status = hdf('H','close',file_id);
    if status == -1
        error('HDF hclose failed')
    end
    % ---------------------------------------------------------------------------------------------------------
    
    
    cd(wd);
    
catch
    hdfml('closeall')
    cd(wd)
    lasterr
end

return
% ---------------------------------------------------------------------------------------------------------


function [startTraj, endTraj, lenTraj] = singleTraj(age)
% Split the file into single trajectories, according to the
% AGE vector, column 29 of the trajPoint.* files
startTraj = find(age == 0);      % start points of the trajectories
endTraj = startTraj(2:end)-1;    % end points of the trajectories
endTraj = [endTraj;length(age)];
lenTraj = endTraj - startTraj + 1;
%    numTraj = length(startTraj);
return



