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
	-- { -- page 3
	   -- "PLACEHOLDER",
	   -- "PLACEHOLDER",
	   -- "PLACEHOLDER",
	   -- "PLACEHOLDER",
	   -- "PLACEHOLDER",
	   -- "PLACEHOLDER",
	   -- "PLACEHOLDER",
	   -- "PLACEHOLDER",
	   -- "PLACEHOLDER",
	   -- "PLACEHOLDER",
	   -- "PLACEHOLDER",
	   -- "PLACEHOLDER",
	-- }
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
		if currentPage > 1 then
			-- get prev page
			currentPage = currentPage - 1
			makerListPage = makerList[currentPage]
			current = 12
			fetchVendorIcons()
		else
			current = 1
		end
	elseif (current + change) > 12 then
		if currentPage < #makerList then
			-- get next page
			currentPage = currentPage + 1
			makerListPage = makerList[currentPage]
			current = 1
			fetchVendorIcons()
		else
			current = 12
		end
	else
		current = current + change
	end
	return current
end

-- get the list of games for the selected build
function gameIndex(current, currentPage)
	mainMenuLoop = false -- invalidate main manu controls
	gameBuildIndex = current + (12 * (currentPage - 1)) -- handle page numbers > 12
	local listofgames = require("gamelist")
	GameList = listofgames.getGameList(gameBuildIndex)
	highlightedGame = 1
	gamesCount = #GameList
	gamesleft = gamesCount -1
	buttons.interval(10,10)
	
	while true do
		buttons.read()
		screen.print(5, (12), ">")
		
		for k=0, math.min(18,gamesleft), 1 do
			if GameList[highlightedGame]["title"] == nil then
				screen.print(15, (12*(k+1)), "No games")
			else
				screen.print(15, (12*(k+1)), GameList[highlightedGame+k]["title"])
			end
		end
		
		screen.print(15, 255, "O : start  X : back", fontSize, fontColor)
		
		-- submenu controls
		if (buttons.down and gamesleft > 0) then
			highlightedGame = highlightedGame + 1
			gamesleft = gamesCount - highlightedGame
		end
		if (buttons.right and gamesleft > 18) then
			highlightedGame = highlightedGame + 18
			gamesleft = gamesCount - highlightedGame
		end
		if (buttons.up and highlightedGame > 1) then
			highlightedGame = highlightedGame - 1
			gamesleft = gamesCount - highlightedGame
		end
		if (buttons.left and highlightedGame > 18) then
			highlightedGame = highlightedGame - 18
			gamesleft = gamesCount - highlightedGame
		end
		if buttons.cross then
			mainMenuLoop = true
			buttons.interval()
			screenRefresh()
			return 0
		end
		-- save choice to ms, to be picked up by pspMAME
		if buttons.circle then
			file = io.open("gameindex", "w")
			io.output(file)
			io.write(GameList[highlightedGame]["game_index"] - 1)
			io.close(file)
			buttons.interval()
			return 1
		end

		screen.flip()
	end
end

fetchVendorIcons()
screenRefresh()

 --Bucle principal--
mainMenuLoop = true
while mainMenuLoop == true do
	buttons.read()
	
		if buttons.down then
			current = getCurrent(current, 1)
			screenRefresh()
		end
		if buttons.up then
			current = getCurrent(current, -1)
			screenRefresh()
		end
		if buttons.right then
			current = getCurrent(current, 3)
			screenRefresh()
		end
		if buttons.left then
			current = getCurrent(current, -3)
			screenRefresh()
		end

		---Ejecutando las aplicaciones---		
		if buttons.circle then
			-- display submenu, save gameindex helper file
			gameSelection = gameIndex(current, currentPage)
			if (gameSelection == 1) then
				-- concatenate eboot path
				local ebootPath = "ms0:/PSP/GAME/pspMAME/" .. makerListPage[current] .. ".PBP"
				print("running ", ebootPath)
				game.launch(ebootPath)
			end
		end
end
