-- code from Brujito's Multi Emulator
-- works with OneLua 4 R1

---Cargando imagenes---
seleccion = image.load("./img/selector.png")
menu = image.load("./img/menu.png")

current = 1;

makerList = {
	{ -- page 1
	   "CAPCOM",
	   "CAVE",
	   "CPS1",
	   "DECO",
	   "DYNAX",
	   "IREM",
	   "JALECO",
	   "KANEKO",
	   "KONAMI",
	   "KONAMI2",
	   "NAMCO",
	   "NAMCO2",
	},
	{ -- page 2
	   "NICHIBUTSU",
	   "NINTENDO",
	   "SEGA",
	   "SEGA2",
	   "SEGA3",
	   "SETA",
	   "SNK",
	   "TAITO",
	   "TAITO2",
	   "TECHNOS",
	   "UNIVERSAL",
	   "UPL",
	},
	{ -- page 3
	   "PLACEHOLDER",
	   "PLACEHOLDER",
	   "PLACEHOLDER",
	   "PLACEHOLDER",
	   "PLACEHOLDER",
	   "PLACEHOLDER",
	   "PLACEHOLDER",
	   "PLACEHOLDER",
	   "PLACEHOLDER",
	   "PLACEHOLDER",
	   "PLACEHOLDER",
	   "PLACEHOLDER",
	}
}

fontColor = color.new(255,255,0)
fontSize = 0.7

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
currentPage = 1
local makerListPage = makerList[currentPage]

-- get vendor logo paths for page 1 or page 2
function fetchVendorIcons()
	vendor = {}
	for i=1,12,1 do
		-- must fit inside the selector (90x57)
		vendor[i] = image.load("./img/" .. makerListPage[i] .. ".png")
	end
end

-- reload all screen elements if any button pressed
function screenRefresh()
	image.blit(menu, 3, 0)
	
	screen.print(20, 260, "<- L trigger", fontSize, fontColor)
	screen.print(370, 260, "R trigger ->", fontSize, fontColor)
	screen.print(90, 230,
	"Press O to launch the "  .. makerListPage[current] .. " build", fontSize, fontColor)
	
	-- draw each of the 12 vendor logos
	-- offset is selector icon offset + 7
	for i=1,12,1 do
		image.blit(vendor[i], (selectorPosX[i] + 7), (selectorPosY[i] + 6))
	end
	
	-- selector icon
	image.blit(seleccion, selectorPosX[current], selectorPosY[current])

 	screen.flip()
end

-- make sure selector doesn't go out of range
function getCurrent(current, change)
	if (current + change) < 1 then
		current = 12
	elseif (current + change) > 12 then
		current = 1
	else
		current = current + change
	end
	return current
end

fetchVendorIcons()
screenRefresh()

 --Bucle principal--
while true do
	buttons.read()
	
	---Moviendo el pad arriba y abajo---
	if buttons.down then
		current = getCurrent(current, 1)
		screenRefresh()
	end
	 
	if buttons.up then
		current = getCurrent(current, -1)
		screenRefresh()
	end
	 
	 ---Moviendo el pad arriba y abajo---
	 if buttons.right then
		current = getCurrent(current, 3)
		screenRefresh()
	end
	 
	if buttons.left then
		current = getCurrent(current, -3)
		screenRefresh()
	end
	
	-- get next page
	if buttons.r then
		if currentPage < 3 then
			currentPage = currentPage + 1
			makerListPage = makerList[currentPage]
		end
		current = 1
		fetchVendorIcons()
		screenRefresh()
	end
	
	-- get prev page
	if buttons.l then
		if currentPage > 1 then
			currentPage = currentPage - 1
			makerListPage = makerList[currentPage]
		end
		current = 1
		fetchVendorIcons()
		screenRefresh()
	end

	---Ejecutando las aplicaciones---		
	if buttons.circle then
		-- concatenate eboot path
		local ebootPath = "ms0:/PSP/GAME/pspMAME/" .. makerListPage[current] .. ".PBP"
		print("running ", ebootPath)
		game.launch(ebootPath)
	end
end
