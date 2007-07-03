function varargout = bvgfun(varargin)
% BVGFUN Run an operation on a bvgraph.
%
% This m file is a stub for the mex file.  The mex file is a single point
% of entry for many of the bvgraph c routines.
%
% [n,nz,smem,gmem,offsetmem] = bvgfun('load',filename,offsetstep)
% This call loads a graph (possibly into memory) and returns a set of
% arrays for all the inmemory data.  
% n is the number of rows/columns in the marix, nz is the number of
% nonzeros, smem is the memory required for the structure, gmem is the
% memory required for the graph, and offsetmem is the memory required for
% the offset array.
%
% In the following calls, smem,gmem,and offsetmem are all the arrays
% returned by the initial call.
%
% y = bvgfun('mult',smem,gmem,offsetmem,x) compute A*x where A is the adjacency
% matrix for the graph G.
%
% y = bvgfun('tmult',smem,gmem,offsetmem,x) compute A'*x where A is the
% adjacency matrix for the graph G.
%
% s1 = bvgfun('sum1,smem,gmem,offsetmem) compute the column sums of A
% s2 = bvgfun('sum2',smem,gmem,offsetmem) compute the row sums of A
%
% d = bvgfun('diag',smem,gmem,offsetmem) compute the diagonal of A


% actually, all this function does is compile the mex file and then
% redirect the original call

srcdir = '../../src';
headerdir = '../../include';

srcfiles = {'bitfile.c', 'bvgraph.c', 'bvgraph_iterator.c', 'bvgraphfun.c', 'properties.c', 'util.c'};
files{1} = 'bvgfun.c';
files(end+1:end+length(srcfiles)) = cellfun(@(x) sprintf('%s/%s', srcdir, x), srcfiles, 'UniformOutput', 0);
% notice the space on the end for the correct catenation
files = cellfun(@(x) sprintf('%s ', x), files, 'UniformOutput',0);
p = mfilename('fullpath');
[path,name,ext,version] = fileparts(p);
olddir = cd;
cd(path);
try
    mexcmd = sprintf('mex -O -I%s %s', headerdir, [files{:}]);
    eval(mexcmd);
    cd(olddir);
catch
    cd(olddir);
    rethrow(lasterr);
end

error('bvgfun:notCompiled', ...
    'the bvgfun was not compiled, please rerun your command as the function was just compiled');
