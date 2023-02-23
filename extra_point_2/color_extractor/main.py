from pickletools import optimize
from statistics import quantiles
from PIL import Image
import os

def RGBCONVERT(num):
    return ( num & 255 , (num >> 8) & 255 , (num >> 16) & 255)

def RGB565CONVERT(red, green, blue):
    return ( (( red   >> 3 ) << 11 ) | (( green >> 2 ) << 5  ) | ( blue  >> 3 ))

fout = open("eating-out.txt" , "w")
fout.write("{\n")
gif = Image.open('eating.gif')

for frame in range(1, gif.n_frames):
    if( frame % 2 == 0):
        continue
    gif.seek(frame)
    new_file_name = 'frame_{}.png'.format(frame)
    gif.save('eating/' + new_file_name , 'png' , optimize = True )
    gif.convert('P')
    fout.write("\t{//"+ new_file_name +"\n")
    print(gif)
    px = gif.load()
    for y in range(gif.height):
        fout.write("\t\t{")
        for x in range (gif.width):
            pixel = list ( px[x,y] ) 
            res = RGB565CONVERT(pixel[0] , pixel [1] , pixel[2] )
            #print(hex(res))
            fout.write(hex(res) + ",")
        fout.write("\t\t},\n")
    fout.write("\t},\n")
fout.write("}")
fout.close()
