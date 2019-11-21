#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

// --------------------------------------------------------
// Upper and lower variables are established for random calculations.
// The 1 - 10 bounds are used to calculate 7 rooms from 10
// --------------------------------------------------------
int upper = 10;
int lower = 1;
int upperRoom = 6;
int lowerRoom = 1;

// --------------------------------------------------------
// The following variables are also established
// Char variable array used to hold directory name.
// Char array pointers containing the 10 total rooms.
// Char array pointers containing the three room types.
// --------------------------------------------------------
char foldName[100];
char* roomNames[10] = {"BLUE", "GREEN", "RED", "WHITE", "BLACK", "YELLOW", "ORANGE", "BROWN", "GOLD", "SILVER"};
char* roomType[3] = {"START_ROOM", "END_ROOM", "MID_ROOM"};

// --------------------------------------------------------
// Room structure is established containing the following
//  - ID
//  - Room TYPE
//  - Name
//  - Type
//  - Number of Connections
//  - Array of pointers to other room structures
// --------------------------------------------------------
struct room {
    int id;
    int typeRoom;
    char* name;
    char* type;
    int numConnections;
    struct room *connections[6];
};

// --------------------------------------------------------
// graph Structure is established containing the following
//  - Number of rooms
//  - Array of room seven structures
// --------------------------------------------------------
struct graph {
    int numRooms;
    struct room map[7];
};


// --------------------------------------------------------
// Function orptotypes are declared
// --------------------------------------------------------
struct room GetRandomRoom();
void generateGraph(struct graph* graph);
void AddRandomConnection(struct graph* graph);
struct room GetRandomRoom();
_Bool CanAddConnectionFrom(struct room* x);
void ConnectRoom(struct room* x, struct room* y);
_Bool IsSameRoom(struct room* x, struct room* y);
void createFiles(struct graph* graph, char* folder);


// --------------------------------------------------------
// CREATE ROOM FUNCTION IS ESTABLISHED
//      This Function takes in a pointer to a name. The name is
//      used to create a new room. The number of connections for
//      the new room is set to zero.
//      The room is then returned.
// --------------------------------------------------------
struct room createRoom(char* name) {
    struct room newRoom;
    newRoom.name = name;
    newRoom.numConnections = 0;
    return newRoom;
}

// --------------------------------------------------------
// GET RANDOM ROOM FUNCTION IS ESTABLISHED
// The function itterates through the array containing the ten rooms array
// A random value is calculated using the formula val = (rand()%(upper-lower+1))+lower
// The room created using the create room function.
// The returned room is then returned.
// --------------------------------------------------------
struct room GetRandomRoom(){
    static int numRoomsCreated = 0;
    int val = (rand() % (upper - lower + 1)) + lower;
    val--;
    struct room newRoom = createRoom(roomNames[val]);
    numRoomsCreated++;
return newRoom;
}

// --------------------------------------------------------
// GENERATE GRAPH FUNCTION IS ESTABLISHED
// The function takes in the pointer to the graph structure and generates seven random rooms
// The rooms are selected randomly using the GetRandomRoom function and added to a room A
// The room is the allocated to a position in the map from 1 to 7.
// The position in the array map will later be used to identify the start and end rooms.
// If the function pulls in a room that is already allocated in the map, the GetRandomRoom
// function is called again until the retrieved room is unique.
// --------------------------------------------------------
void generateGraph(struct graph* graph) {
    int itterate = 0;
    int doubleRoom = 0;
    int idNum = 1;
    while(graph->numRooms < 7){
        doubleRoom = 0;
        struct room A = GetRandomRoom();
        if(itterate > 0){
            int b = 0;
            for(b = itterate - 1; b >= 0; b--){
                doubleRoom = 0;
                doubleRoom = IsSameRoom(&A, &graph->map[b]);
                if (doubleRoom == 1){
                    break;
                }
            }
            if (doubleRoom == 1){
                continue;
            }
        }
        graph->numRooms++;
        graph->map[itterate] = A;
        graph->map[itterate].id = idNum;
        idNum++;
        itterate++;
    }
}

// --------------------------------------------------------
//  ADD RANDOM CONNECTION FUNCTION IS ESTABLISHED
// ****Adds a random, valid outbound connection from a Room to another Room****
// The function takes in a pointer to the structure graph containing the 7 rooms.
// The function creates the connections within the seven rooms utilizing the following
// functions.
//      GetRandomRoom(), CanAddConnectionFrom(), ConnectRoom()
// The function pulls in two rooms, verifies that they are not the same room,
// checks to see if they have already been connected, and ensures a connection can be established.
// If this results in true, a connection is created between the two selected rooms.
// --------------------------------------------------------
void AddRandomConnection(struct graph* graph){
    int ittr = 0;
    struct room A;
    struct room B;
    int loop = 1;
    int valueA;
    int valueB;
    int looper = 1;

// Loop is initialized until a connection is made between two rooms
    while (loop){
        int returnVal = 0;
        int returnValTwo = 0;
        int retValThree;
        int outerLoop = 1;
        int innerLoop = 1;
        int twoloop = 1;

// The following loops are initialized to pull a room that can have a connection added
// The loop continues until a room is successfully pulled
        while (outerLoop){
            while (innerLoop){
                A = GetRandomRoom();
                int x = 0;
                for (x = 0; x < 7; x++) {
                    if((graph->map[x].name) == A.name)
                    {
                        valueA = x;
                        innerLoop = 0;
                        break;
                    }
                }
            }
            returnVal = CanAddConnectionFrom(&graph->map[valueA]);
            if (returnVal == 1){
                outerLoop = 0;
            }
        }

// The following loop is initialized to pull a room that can have a connection added
// The loop continues until a room is successfully pulled
        while (twoloop){
            B = GetRandomRoom();
            int x = 0;
            for (x = 0; x < 7; x++) {
                if((graph->map[x].name) == B.name)
                {
                    valueB = x;
                    returnValTwo = CanAddConnectionFrom(&graph->map[valueB]);
                    if(returnValTwo == 1){
                        twoloop = 0;
                        break;
                    }
                }
            }
        }

// The following loops are used to compare the names of the two rooms selected
// The connections of each room are compared to make sure the connections are not duplicated
// If any of these fail, the loop begins again
// If these pass, the loop ends and the rooms are connected
        int check = 0;
        int i = 0;
        for(i = 0; i < graph->map[valueA].numConnections; i++){
            if ((strcmp(graph->map[valueA].connections[i]->name, graph->map[valueB].name)) == 0){
                check = 1;
            }
            else if ((strcmp(graph->map[valueA].connections[i]->name, graph->map[valueB].name)) != 0){
            }
        }

        if ((strcmp(graph->map[valueA].name, graph->map[valueB].name)) != 0){
            if (check == 0)
            {
                ittr++;
                loop = 0;
            }
        }
    }
    
// Rooms are connected
    ConnectRoom(&graph->map[valueA], &graph->map[valueB]);
    ConnectRoom(&graph->map[valueB], &graph->map[valueA]);
}

// --------------------------------------------------------
//  CAN ADD CONNECTION FUNCTION IS ESTABLISHED
// ****// Returns true if a connection can be added from Room x (< 6 outbound connections), false otherwise****
// --------------------------------------------------------
_Bool CanAddConnectionFrom(struct room* x){
    if (x->numConnections <= 6){
        return 1;
    }
    else{
        return 0;
    }
}

// --------------------------------------------------------
//  CONNECT ROOM FUNCTION IS ESTABLISHED
// ****Connects Rooms x and y together, does not check if this connection is valid****
// --------------------------------------------------------
void ConnectRoom(struct room* x, struct room* y){
    x->connections[x->numConnections] = y;
    x->numConnections++;
}


// --------------------------------------------------------
//  CONNECT ROOM FUNCTION IS ESTABLISHED
// ****Returns true if Rooms x and y are the same Room, false otherwise****
// --------------------------------------------------------
_Bool IsSameRoom(struct room* x, struct room* y){
    if (strcmp(x->name, y->name) == 0) {
        return 1;
    }
    else {
        return 0;
    }
}

// --------------------------------------------------------
//  CREATE FILES FUNCTION IS ESTABLISHED
// The function takes in two pointers, the graph structure and the
// The cunction itterates through the enire structure printing the contents of the files
// The program opens the files prints the ROOM NAME, CONNECTIONS, and ROOM TYPE.
// The program then closes the files
// This continues throughout the entire structure.
// --------------------------------------------------------
void createFiles(struct graph* graph, char* folder){
    chdir(folder);
    int i;
    for(i = 0; i < 7; i++){
        FILE* myFile = fopen(graph->map[i].name, "a");
        fprintf(myFile, "ROOM NAME: %s\n", graph->map[i].name);

        int j;
        for(j = 0; j < graph->map[i].numConnections; j++){
            fprintf(myFile, "CONNECTION %d: %s\n", j + 1, graph->map[i].connections[j]->name);
        }

        fprintf(myFile, "ROOM TYPE: %s\n", graph->map[i].type);
        fclose(myFile);
    }
}

// --------------------------------------------------------
//  MAKEFOLDER FUNCTION IS ESTABLISHED
// --------------------------------------------------------
char* makeFolder(){
  sprintf(foldName, "salinari.rooms.%d", getpid());
  mkdir(foldName, 0777);
  return foldName;
}

// --------------------------------------------------------
//  MAIN IS ESTABLISHED
// --------------------------------------------------------
int main(){
    srand(time(0));             //srand time is declared for random calculations
    int looping = 1;            //Looping for creating rooms is initialized
    struct graph graph;         //Graph structure is established
    graph.numRooms = 0;

// Generate graph function is calculated
// Graph returns with 7 random rooms identified.
    generateGraph(&graph);
    int count = 15;
    int check = 0;      // int check is initialized

// While loop is initiated while check is is not equal to 7
// The loop continues to call random rooms and creating connections until
// the for loop can run through the entire graph and every room meets the
// minimum connection requirements.
// In other words, after a connection is made, all of the rooms are checked.
// If not all pass, another connection is made.
    while(looping){
        check = 0;
        AddRandomConnection(&graph);
        int b = 0;
        for (b = 0; b < 7; b++){
            if ((graph.map[b].numConnections > 2) && (graph.map[b].numConnections < 7)){
                check++;
            }
        }
        count--;
        if (check == 7){
            looping = 0;
            break;
        }
    }

// The following for loop creates the types for the rooms
// The first room identified or map[0] is set to start room
// The last room identified or map[6] is set to end rooms
// Every other room is set to mid room
    int x = 0;
    for (x = 0; x < 7; x++) {
        if (graph.map[x].id == 1) {
            graph.map[x].type = roomType[0];
        }
        else if ((graph.map[x].id > 1) && (graph.map[x].id < 7)) {
            graph.map[x].type = roomType[2];
        }
        else if (graph.map[x].id == 7) {
            graph.map[x].type = roomType[1];
        }
    }
// Directory is created using the makeRooms function
// The files are then created using the graph and createFiles function.
    char* roomsFolder = makeFolder();
    createFiles(&graph, roomsFolder);
return 0;
}
