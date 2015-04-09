function [traj,trajLen] = ptv2traj_v2(ptv,minLength,dt)
% ptv2TRAJ_v2 converts structure from ptv where every element is the particles
% in a specific time (i.e. per frame, with trajectory id's)
% to the structure TRAJ in which every element of the structure is a
% trajectory in time and space.

% Author: Alex Liberzon, alex.dot.liberzon.at.gmail.dot.com
% Created: 2-Apr-2011
% Last modified Dec 1, 2012
% - should work using sortrows and diff of the last column approach
% - fixed the bug when no -999 appeared
% - added close(h) to remove the waitbar

if nargin < 2
    minLength = 3;
    dt = NaN;
elseif nargin < 3
    dt = NaN;
end


% this moved into original reading of ptv_is_files
% probably one needs a fix for other files, e.g. xuap
% Alex, 31-Jan-2013

% for i = 1:length(ptv)
%     if length(ptv(i).xr) > 1
%         ptv(i).t = repmat(ptv(i).t,[length(ptv(i).xr),1]);
%     end
% end

if isnan(dt)
    dt = min(diff(t));
end
try
    tmp = cat(2,cat(1,ptv.xr),cat(1,ptv.yr),cat(1,ptv.zr),cat(2,ptv.trajid).',cat(1,ptv.t));
catch
    tmp = cat(2,cat(1,ptv.xr),cat(1,ptv.yr),cat(1,ptv.zr),cat(1,ptv.trajid),cat(1,ptv.t));
end
tmp = sortrows(tmp,4);
if any(tmp(:,4) == -999)
    tmp = tmp(find(tmp(:,4)==-999,1,'last')+1:end,:); % cut out -999
end
[len,~,first,last] = mttrlencode(tmp(:,4));
idl = find(len > minLength); % only long ones

traj = repmat(struct('x',[],'y',[],'z',[],'u',[],'v',[],'w',[],...
    'ax',[],'ay',[],'az',[],'t',[],'trajid',[]),length(idl),1);

trajLen = zeros(length(idl),1);
h = waitbar(0,'Please wait ...');



for k = 1:length(idl)
    
    i = idl(k);
    waitbar(k/length(idl),h);
    
    traj(k).xf = tmp(first(i):last(i),1);
    traj(k).yf = tmp(first(i):last(i),2);
    traj(k).zf = tmp(first(i):last(i),3);
    traj(k).trajid  = tmp(first(i):last(i),4);
    traj(k).t  = tmp(first(i):last(i),5);
    
%     [~,traj(k).x] = spaps(traj(k).t,traj(k).x,0);
%     [~,traj(k).y] = spaps(traj(k).t,traj(k).y,0);
%     [~,traj(k).zf] = spaps(traj(k).t,traj(k).zf,0);

    [traj(k).xf,traj(k).uf,traj(k).axf] = smoothspline(traj(k).t*dt,traj(k).xf);
    [traj(k).yf,traj(k).vf,traj(k).ayf] = smoothspline(traj(k).t*dt,traj(k).yf);
    [traj(k).zf,traj(k).wf,traj(k).azf] = smoothspline(traj(k).t*dt,traj(k).zf);
    
%     traj(k).uf = gradient5(traj(k).x,traj(k).t*dt);
%     traj(k).v = gradient5(traj(k).y,traj(k).t*dt);
%     traj(k).wf = gradient5(traj(k).z,traj(k).t*dt);
%     
%     traj(k).ax = gradient5(traj(k).u,traj(k).t*dt);
%     traj(k).ayf = gradient5(traj(k).v,traj(k).t*dt);
%     traj(k).az = gradient5(traj(k).wf,traj(k).t*dt);
    
    trajLen(k) = len(i);
end
close(h)

function [ys,df,ddf] = smoothspline(x,y)
% w = ones(size(x)); w([1 end]) = 1.e-5;
% [sp,ys] = spaps(x,y, 1.e-2, w, 3);
% ys = smooth1q(y,'dct');
% [sp,ys] = spaps(x,y,1);
% h = x(2) - x(1);
% p = 1/(1 + h^3/6);
% [sp] = csaps(x,y,p);
% sp = csape(x,y,'variational');

l = min(21,floor(length(x)/5));
sp = spap2(l,4,x,y);
ys = fnval(sp,x);
df = fnval(fnder(sp),x);
ddf = fnval(fnder(sp,2),x);



