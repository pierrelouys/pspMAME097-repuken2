-- code from Brujito's Multi Emulator

---Cargando imagenes---
seleccion = Image.load("./img/selector.png")

menuPage01 = Image.load("./img/menu01.png")
menuPage02 = Image.load("./img/menu02.png")

current = 1;

makerListPage01 = {
   "CAPCOM", --1
   "DECO", --2
   "IREM", --3
   "KONAMI", --4
   "NAMCO", --5
   "NICHIBUTSU", --6
   "NINTENDO", --7
   "SEGA", --8
   "SNK", --9
   "TECHNOS", --10
   "UNIVERSAL", --11
   "UPL", --12
}

makerListPage02 = {
   "TAITO", --1
   "PLACEHOLDER", --2
   "PLACEHOLDER", --3
   "PLACEHOLDER", --4
   "PLACEHOLDER", --5
   "PLACEHOLDER", --6
   "PLACEHOLDER", --7
   "PLACEHOLDER", --8
   "PLACEHOLDER", --9
   "PLACEHOLDER", --10
   "PLACEHOLDER", --11
   "PLACEHOLDER", --12
}

fontColor = Color.new(255,255,0,0)

-- set x,y offset for the selection icon
local selectorPosX = {}
local selectorOffsetX = 40
for i=1,12,3 do
	selectorPosX[i] = selectorOffsetX
	selectorPosX[i+1] = selectorOffsetX
	selectorPosX[i+2] = selectorOffsetX
	selectorOffsetX = selectorOffsetX + 100
end

local selectorPosY = {}
local selectorOffsety = nil
for i=1,12,3 do
	selectorPosY[i] = 5
	selectorPosY[i+1] = 78
	selectorPosY[i+2] = 151
end

-- assign content from page 1
local menu = menuPage01
local makerList = makerListPage01

 --Bucle principal--
while true do
	screen:clear()
	pad = Controls.read()
	screen:blit(3,0, menu)
	screen:print(20, 260, "<- L trigger", fontColor)
	screen:print(370, 260, "R trigger ->", fontColor)
	screen:print(90, 230,
	"Press X to launch the "  .. makerList[current] .. " build", fontColor)
	
	---Moviendo el pad arriba y abajo---
	if pad:down() then
		current = current + 1
		System.sleep(200)-- pone una pausa de 200 milisegundos para que al pulsar no baje muy rapido la seleccion--	
	end
	 
	if pad:up() then
		current = current - 1
		System.sleep(200)		
	end
	 
	 ---Moviendo el pad arriba y abajo---
	 if pad:right() then
		current = current + 3
		System.sleep(200)		
	end
	 
	if pad:left() then
		current = current - 3
		System.sleep(200)		
	end
	
	---Aqui depende de la cantidades de opciones yo tengo 12---	 
	if current < 1 then---aqui dice XD current es menor a 1---
		current = 12
	end
	 
	if current > 12 then -- aqui dice current es mayor a 12---
		current = 1
	end
	
	-- get page 2 / back to page 1
	if pad:r() then
		menu = menuPage02
		makerList = makerListPage02
		current = 1
		System.sleep(200)		
	end
	
	if pad:l() then
		menu = menuPage01
		makerList = makerListPage01
		current = 1
		System.sleep(200)		
	end
	 
	---Opciones del menu y para que ponga tu imagen de seleccion en una de las opciones a elejir---  
	screen:blit(selectorPosX[current], selectorPosY[current], seleccion)

	---Ejecutando las aplicaciones---		
	if pad:cross() then
		-- concatenate eboot path
		local ebootPath = "ms0:/PSP/GAME/pspMAME/" .. makerList[current] .. ".PBP"
		print("running ", ebootPath)
		System.runeboot(ebootPath)
	end

 	screen.flip()
	screen.waitVblankStart()
end
