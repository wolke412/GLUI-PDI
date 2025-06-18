from PIL import Image
import time

# +-----------------------------------------------------------
# | Classe pra fazer o benchmarking dessa merda mega lenta
# +-----------------------------------------------------------
class StepTimer:
    def __init__(self):
        self.start_time = time.perf_counter()
        self.last_time = self.start_time

    def step(self, label=""):
        now = time.perf_counter()
        duration = now - self.last_time
        print(f"[{label}] Step took: {duration:.6f} seconds")
        self.last_time = now

    def total(self):
        total_duration = time.perf_counter() - self.start_time
        print(f"Total time: {total_duration:.6f} seconds")
# ------------------------------------------------------------

timer = StepTimer()

#img = Image.open("1.png")
#img = Image.open("2.png")
# img = Image.open("3.png")
# img = Image.open("4.png")
img = Image.open("5.png")

THRESHOLD = 254

w, h = img.size

print(f"Opened an imagem of [{w} x {h}]")

timer.step("Open image")

class Img:
    def __init__( self, w, h, pxmtx ):
        self.w = w
        self.h = h
        self.mtx = pxmtx

    def getpx( self, x = 0, y = 0 ):
        return self.mtx[ x + y * self.w ] 

    # coun '1' fields
    def connectivity( self, x, y ):
        offsets = [ 
            (0, -1), 
            (1 ,0), 
            (0, 1), 
            (-1, 0) 
        ] 
        c = 0
        for offset in offsets:
            xo, yo = offset
            if self.getpx( x + xo, y + yo ):
                c+=1

binmtx = []
for y in range( h ):
    for x in range( w ):
        px = img.getpixel((x, y))
        binmtx.append(1 if px[2] > THRESHOLD else 0)


timer.step("Apply Threshold")

class MaskMatrix:
    def __init__( self, w, h ):
        self.w = w
        self.h = h
        self.mtx = [ 0 for _ in range( w * h ) ] 

    def getat(self, x=0, y=0):
        idx = x + y * self.w
        val = self.mtx[idx]
        return val

    def setat(self, x=0, y=0, val=1):
        idx = x + y * self.w
        self.mtx[idx] = val

#
#
class Shape:
    def __init__(self, shapen, pixels: list[tuple[int, int]] ):
        self.id = shapen 
        self.pixels = pixels

    def setpx( self, x, y ):
        self.pixels.append((x, y))

    def has(self):
        return len(self.pixels) > 0

offsets = [ 
            (0, -1), 
            (1 ,0), 
            (0, 1), 
            (-1, 0) 
        ] 
#
#
def dfs(img, start_x, start_y, shape, mask):
    stack = [(start_x, start_y)]
    mask.setat(start_x, start_y, shape.id)  

    while stack:
        x, y = stack.pop()
        shape.setpx(x, y)

        for dx, dy in offsets:
            nx, ny = x + dx, y + dy
            if nx < 0 or ny < 0 or nx >= img.w or ny >= img.h:
                continue
            if img.getpx(nx, ny) != 1:
                continue
            if mask.getat(nx, ny) != 0:  
                continue

            mask.setat(nx, ny, shape.id)  
            stack.append((nx, ny))

shapes = []
pimg = Img( w, h, binmtx )
mask = MaskMatrix( w, h )

c = 1
for y in range(pimg.h):
    for x in range(pimg.w):
        px = pimg.getpx( x, y )
        if  px == 1 and not mask.getat( x, y ):
            currentshape = Shape(c, [])
            dfs( pimg, x, y, currentshape, mask )
            if currentshape.has():
                shapes.append(currentshape)
                c+=1

timer.step("Detect shapes")

print(f"FORMAS: {c}")

# for sh in shapes:
#     print(f"shape {sh.id} composed of {len(sh.pixels)}")

# !!!! PERGUNTAR PRA PORF MARTA O QUE PODE TER ACONTECIDO AQUI!
# aparentemente é pq eu não processei perfeitamente a imagem;
# protanto, foram gerados grupos de menos de 20 pixeis...
# mas tudo certo, resolvido.
shapes = [ s for s in shapes if len(s.pixels) > 20 ]

timer.step("Filter shapes")

class BR:
    def __init__ (self):
        self.min_x = 10000000
        self.min_y = 10000000
        self.max_x = 0
        self.max_y = 0
        self.centroid = 0

def get_bounding_rect( s: Shape ) -> BR:
    br = BR()
    totalx = 0
    totaly = 0
    for (x,y) in s.pixels:
        totalx += x
        totaly += y

        if x > br.max_x:
            br.max_x = x
        if y > br.max_y:
            br.max_y = y
            
        if x < br.min_x:
            br.min_x = x
        if y < br.min_y:
            br.min_y = y
    
    br.centroid = ( totalx / len(s.pixels), totaly / len(s.pixels) )
    return br


def get_white_area_percentage( rect, img: Img ):
    rx, ry, w, h = rect
    totalpx = w * h
    whitepx = 0
    for x in range(w):
        for y in range(h):
            px = img.getpx(rx + x, ry+y)
            if px:
                whitepx += 1

    return whitepx / totalpx



TOLERANCE = 5

redondos = 0
pilulas  = 0
quebrados  = 0
for shape in  shapes:
    br = get_bounding_rect( shape )
    # print(f"Mx == { br.max_x }")
    # print(f"My == { br.max_y }")
    # print(f"mx == { br.min_x }")
    # print(f"my == { br.min_y }")
    # print(f"ce == { br.centroid}")
    dx = br.max_x - br.min_x
    dy = br.max_y - br.min_y
    # print(f"diff_x == { dx }")
    # print(f"diff_y == { dy }")

    p = get_white_area_percentage(
        ( br.min_x, br.min_y, br.max_x - br.min_x, br.max_y - br.min_y ), pimg
    )

    print(f"percentarea == { p }")

    LIM_PILULA_INF = 0.6
    LIM_PILULA_SUP = 0.9 # isso teria que ser confirmado

    if abs( dx - dy ) > TOLERANCE:
        if LIM_PILULA_SUP >= p > LIM_PILULA_INF :
            quebrados += 1
        else:
            pilulas+=1
    else:
        redondos+=1


timer.step("Do math")

print(f" {len(shapes)} Comprimidos estão na esteira")
print(f" {pilulas} Pilulas")
print(f" {redondos} Redondos")
print(f" {quebrados} Quebrados")

