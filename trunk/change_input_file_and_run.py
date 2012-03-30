import glob
import os

folders = glob.glob('res_*')

for folder in folders[15:]:
    files = glob.glob(folder+os.sep+'ptv_is.*')
    first = files[0].rsplit('.')[1]
    last  = files[-1].rsplit('.')[1]

    print folder, first, last

    f = open('input.inp','w')
    f.writelines(['1','\n','1','\n','1','\n','0\n','0\n'])
    f.writelines([folder,'\n',first,'\n',last,'\n'])
    f.writelines(['0.00625\n0.000001\n21\n3\n0.13\n0.010\n0.01\n0.99\n'])
    f.writelines('--- \n')
    f.writelines(['3\n35\n10.0\n0.05\n199\n200\n'])
    f.close()
    os.system('post_process.exe &')

    
