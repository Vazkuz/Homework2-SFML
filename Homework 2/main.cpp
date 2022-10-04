#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Main.hpp>
#include <iostream>
#include <fstream>

#include <typeinfo>

using namespace sf;
using namespace std;

//Global variables
const int spriteSize = 70, nRows = 7, nCols = 21; // Initializing the dimensions of the tileset
int tileIndexRow = 0, tileIndexCol = 0;  // Initializing the indices of the tiles. These are the indices corresponding to each sprite in the tileset.
const int windowTileWidth = 15, windowTileHeight = 10; // Size of the window will be in relation to the size of the tiles
Sprite tileSprite; // The Sprite being rendered on the cursor
Image tileset; // The Image of the whole tileset
Texture tileTextureArray[nRows][nCols]; // An array containing the textures of all the tiles available
bool hasTileChanged = false; // Boolean that will be used to update the tilemap and level sprites in the frame when the player clicks the window.
bool hasTileBeenErased = false; // Boolean that will be used to erase the tile when the player right clicks the window.

int tileMap[windowTileWidth][windowTileHeight]; // This tileMap will be used when saving the level to or loading the level from a txt file
Sprite levelSprite[windowTileWidth][windowTileHeight]; // Sprite array for all the window
const Sprite emptySprite; // Empty Sprite (will be used when erasing a Sprite). Const because we don't want it to have any assignment in the future.

//Prototypes
void handleInput(RenderWindow& window, Event e);
void update(RenderWindow& window);
void render(RenderWindow& window);
void handleUp();
void handleDown();
void saveScreenshot(const RenderWindow& window);
void saveLevel();
void loadLevel();
void updateTileMap(int cursorGridPosX, int cursorGridPosY, float posX, float posY);
void eraseTile(int cursorGridPosX, int cursorGridPosY);


int main()
{

    RenderWindow window(VideoMode(windowTileWidth * spriteSize, windowTileHeight * spriteSize), "SFML works!");
    window.setFramerateLimit(144);

    // Load an image file from a file
    if (!tileset.loadFromFile("Tileset/Platformer-70x70.png"))
        return -1;

    // All initial values in the tile map are -1 (which means there's no sprite in that grid position)
    for (int i = 0; i < windowTileHeight; i++) {
        for (int j = 0; j < windowTileWidth; j++) {
            tileMap[j][i] = -1;
        }
    }

    // All tiles are saved in an array to be used later when the player is choosing one to create their level
    for (int i = 0; i < nRows; i++) {
        for (int j = 0; j < nCols; j++) {
            if (!tileTextureArray[i][j].loadFromImage(tileset, IntRect(j * spriteSize, i * spriteSize, spriteSize, spriteSize))) return -1;
        }
    }

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            handleInput(window, event);
        }

        update(window);

        render(window);
    }

    return 0;
}

// =======================================================================================================
// =========================================HANDLE INPUT FUNCTION=========================================
// =======================================================================================================
void handleInput(RenderWindow& window, Event e) {
    // Closing window
    if (e.type == Event::Closed)
        window.close();

    // Changing the tile up
    if (Keyboard::isKeyPressed(Keyboard::Up)) {
        handleUp();
    }

    // Changing the tile down
    if (Keyboard::isKeyPressed(Keyboard::Down)) {
        handleDown();
    }

    // Placing the current tile on the grid cell
    if (e.type == Event::MouseButtonPressed && e.mouseButton.button == Mouse::Left) {
        // When the player clicks on the window, this boolean is set to true (updating the tilemap and the sprites on the level will be handled in Update)
        hasTileChanged = true;
    }

    // Erasing the tile
    if (e.type == Event::MouseButtonPressed && e.mouseButton.button == Mouse::Right) {
        // When the player right clicks on the window, the sprite (if any) on that grid cell will be erased in the Update loop. This boolean will trigger that.
        hasTileBeenErased = true;
    }

    // Saving screenshot by pressing 'Space'
    if (Keyboard::isKeyPressed(Keyboard::Space)) {
        saveScreenshot(window);
    }

    // Saving level by pressing 'S'
    if (Keyboard::isKeyPressed(Keyboard::S)) {
        saveLevel();
    }

    // Loading level by pressing 'L
    if (Keyboard::isKeyPressed(Keyboard::L)) {
        loadLevel();
    }

}

// =======================================================================================================
// ============================================UPDATE FUNCTION============================================
// =======================================================================================================
void update(RenderWindow& window) {
    tileSprite.setTexture(tileTextureArray[tileIndexRow][tileIndexCol]);
    // Position of tiles is fixed in a grid. posX and posY are the positions in the window, while cursorGridPosX and cursorGridPosY are the positions in the grid
    float posX = (Mouse::getPosition(window).x) - (int)(Mouse::getPosition(window).x) % spriteSize;
    float posY = (Mouse::getPosition(window).y) - (int)(Mouse::getPosition(window).y) % spriteSize;
    int cursorGridPosX = (int)(posX / 70);
    int cursorGridPosY = (int)(posY / 70);

    tileSprite.setPosition(posX, posY);

    if (hasTileChanged) {
        // We update the tilemap array only when the tile has changed (when the player clicks)
        updateTileMap(cursorGridPosX, cursorGridPosY, posX, posY);
        // Set boolean to false so this section is only run when the player clicks the window
        hasTileChanged = false;
    }

    if (hasTileBeenErased) {
        eraseTile(cursorGridPosX, cursorGridPosY);
        hasTileBeenErased = false;
    }
}

// =======================================================================================================
// ============================================RENDER FUNCTION============================================
// =======================================================================================================
void render(RenderWindow& window) {
    window.clear();

    // Render all sprites on the window
    for (int i = 0; i < windowTileHeight; i++) {
        for (int j = 0; j < windowTileWidth; j++) {
            window.draw(levelSprite[j][i]);
        }
    }
    // Render the sprite selected by player
    window.draw(tileSprite);

    window.display();

}


void handleUp()
{
    // Up by 1
    tileIndexRow += 1;
    //  If we've reached the last row of tiles, we need to go to the first row and next column
    if (tileIndexRow >= nRows) {
        tileIndexRow = 0;
        tileIndexCol += 1;
        // If we've reached the last column, then we need to go to the first tile (row and column)
        if (tileIndexCol >= nCols) {
            tileIndexCol = 0;
        }
    }
}
void handleDown()
{
    // Down by 1
    tileIndexRow -= 1;
    //If we've reached the first row of tiles, we need to go to the last row and previous column
    if (tileIndexRow <= 0) {
        tileIndexRow = nRows-1;
        tileIndexCol -= 1;
        // If we've reached the first column, then we need to go to the last tile (row and column)
        if (tileIndexCol <= 0) {
            tileIndexCol = nCols-1;
        }
    }
}
void saveScreenshot(const RenderWindow& window) {
    Texture texture;
    texture.create(window.getSize().x, window.getSize().y);
    texture.update(window);
    string filename;
    cout << "Enter a name for your screenshot: ";
    cin >> filename;
    texture.copyToImage().saveToFile("Screenshots/" + filename);
    std::cout << "Screenshot saved to " << "Screenshots/" << filename << std::endl;
}
void saveLevel() {
    string filename;
    cout << "Enter a name to save your level: ";
    cin >> filename;
    ofstream myfile("Levels/"+ filename + ".txt");
    if (myfile.is_open())
    {
        for (int j = 0; j < windowTileHeight; j++) {
            for (int i = 0; i < windowTileWidth; i++) {
                myfile << tileMap[i][j] << " ";
            }
            myfile << '\n';
        }
        myfile.close();
        cout << "File was successfully saved in Levels/" << filename << ".txt" << endl;
    }
    else cout << "Unable to save file";

}
void loadLevel() {
    string filename;
    cout << "Enter a name to open a level: ";
    cin >> filename;
    fstream myfile;
    myfile.open("Levels/" + filename + ".txt", ios::in);
    if (myfile.is_open())
    {
        string tp;
        while (getline(myfile, tp)) { //read data from file object and put it into string.
            vector<string> words{};
            size_t pos = 0;
            //cout << tp << "xd" << "\n"; //print the data of the string
            while ((pos = tp.find(" ")) != string::npos) {
                words.push_back(tp.substr(0, pos));
                tp.erase(0, pos + 1);
            }
            for (const auto& str : words) {
                cout << str << endl;
            }
            cout << endl;
            cout << endl;
            cout << endl;
            cout << words[0] << endl;
        }
        myfile.close(); //close the file object.
        cout << "File was successfully load from Levels/" << filename << ".txt" << endl;
    }
    else cout << "Unable to open file";


}

void updateTileMap(int cursorGridPosX, int cursorGridPosY, float posX, float posY) {
    // Update tilemap
    tileMap[cursorGridPosX][cursorGridPosY] = tileIndexRow * nCols + tileIndexCol;

    // Update texture and sprites on level
    levelSprite[cursorGridPosX][cursorGridPosY].setTexture(tileTextureArray[tileIndexRow][tileIndexCol]);
    levelSprite[cursorGridPosX][cursorGridPosY].setPosition(posX, posY);
}

void eraseTile(int cursorGridPosX, int cursorGridPosY) {
    // First we assign the Sprite to the "emptySprite" so we remove the texture
    levelSprite[cursorGridPosX][cursorGridPosY] = emptySprite;
    // And then we update the tileMap so that position is empty.
    tileMap[cursorGridPosX][cursorGridPosY] = -1;
}