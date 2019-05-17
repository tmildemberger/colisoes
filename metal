disable cpp -Wswitch-default
disable cpp -Wredundant-decls
configlib sdl2
configlib sdl2_image
configlib glm
exec . *.cpp using sdl2 sdl2_image glm