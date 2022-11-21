from PIL import Image
import sys

im = Image.open(sys.argv[1]).convert('L')

# im = im.resize((20,20))

# im.show()
f = open(sys.argv[2], "wb")
f.write(im.tobytes())
f.close()
