from PIL import Image


def RGB565CONVERT(red, green, blue):
    return ( (( red   >> 3 ) << 11 ) | (( green >> 2 ) << 5  ) | ( blue  >> 3 ))
    
img = Image.open('meal.png')
px = img.load()
fout = open("meal-out.txt" , "w")
for y in range(img.height):
    fout.write("{")
    for x in range (img.width):
        pixel = list(px[x,y])
        res = RGB565CONVERT(pixel[0] , pixel [1] , pixel[2] )
        #print(hex(res))
        fout.write(hex(res) + ",")
    fout.write("},\n")
fout.close()



