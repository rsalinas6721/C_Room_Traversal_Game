#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
pthread_mutex_t mutex;

// --------------------------------------------------------
// The following variables are also established
// Char array used to hold rooms traversed.
// Char array pointers containing the 10 total rooms.
// Char array pointers containing the three room types.

// --------------------------------------------------------
char* roomsTraversed[100];
char* roomNames[10] = {"BLUE", "GREEN", "RED", "WHITE", "BLACK", "YELLOW", "ORANGE", "BROWN", "GOLD", "SILVER"};
char* roomType[3] = {"START_ROOM", "END_ROOM", "MID_ROOM"};
char newestDirName[256];

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
    char* name;
    int type;
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
// FIND DIRECTORY FUNCTION IS ESTABLISHED
// *** This function was taken entirely from the lecture Manipulating Directories. ***
// The function itterates through the direcroty looking for files that match salinari.rooms.
// The function identifies the function that contains the newest timestamps
// --------------------------------------------------------
void FindDirectory(){
    int newestDirTime = -1;
    char targetDirPrefix[32] = "salinari.rooms.";
    char* buffer = malloc(sizeof(char) * 64);
    memset(newestDirName, '\0', sizeof(newestDirName));
    DIR* dirToCheck;
    struct dirent *fileInDir;
    struct stat dirAttributes;
    dirToCheck = opendir(".");
    if (dirToCheck > 0)
    {
      while ((fileInDir = readdir(dirToCheck)) != NULL)
      {
        if (strstr(fileInDir->d_name, targetDirPrefix) != NULL)
        {
          stat(fileInDir->d_name, &dirAttributes);
          if ((int)dirAttributes.st_mtime > newestDirTime)
          {
            newestDirTime = (int)dirAttributes.st_mtime;
            memset(newestDirName, '\0', sizeof(newestDirName));
            strcpy(newestDirName, fileInDir->d_name);
          }
        }
      }
    }
    closedir(dirToCheck);
}


// --------------------------------------------------------
//  GENGRAPH FUNCTION IS ESTABLISHED
//  The function itterates through the  files in the directory.
//  The file names are pulled and used to create room structures
// Those room structures are the allocated to the graph.map[] array structure.
// --------------------------------------------------------
void genGraph(struct graph* graph){
    int fileNum = 0;
    int number = 0;
    struct dirent* files;
    DIR* directoryPtr;
    if ((directoryPtr = opendir(newestDirName)) != NULL) {
            while ((files = readdir (directoryPtr)) != NULL) {
                if (strlen(files->d_name) > 2) {
                    struct room newRoom;
                    newRoom.name = files->d_name;
                    graph->map[number] = newRoom;
                    graph->map[number].numConnections = 0;
                    graph->map[number].id = number + 1;
                    graph->numRooms++;
                    number++;
                fileNum++;
                }
            }
        }
}

// --------------------------------------------------------
//  GENGRAPH FUNCTION IS ESTABLISHED
//  The function itterates through the  files in the directory.
//  The function then searches the first characters of every line for the appropriate room type
//  The room types are then annotated using an integer within the graph.map structure
// --------------------------------------------------------
void populateFiles(struct graph* graph){
// Rooms directory is entered.
    chdir(newestDirName);
    int result = 0;
    char fileName[256];
    FILE *fp;
    int c;
    int i;

// Every file is opened
// The first 19 and 21 characters are compared to the appropriate room roomNames
// If a match is found, the type is assigned to the room
    for(i = 0; i < 7; i++){
        fp = fopen(graph->map[i].name,"r");
        if (fp == NULL){
            printf("An Error Occured");
        }
            while(fgets(fileName, sizeof(fileName),fp) != NULL) {
                    if ((strncmp(fileName, "ROOM TYPE: MID_ROOM", 19)) == 0){
                        graph->map[i].type = 2;
                    }
                    else if ((strncmp(fileName, "ROOM TYPE: START_ROOM", 21)) == 0){
                        graph->map[i].type = 1;
                    }
                    else if ((strncmp(fileName, "ROOM TYPE: END_ROOM", 19)) == 0){
                        graph->map[i].type = 3;
                    }
                }
// Files are closed
            fclose(fp);
        }
// Directory is exited
    chdir("..");
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
//  GENGRAPH FUNCTION IS ESTABLISHED
//  The function itterates through the  files in the directory.
//  The function then searches the first characters of every line for the word CONNECTION
//  The word after the string connection is then retrieved from every line
//  The name is used with the ConnectRoom function to establish connections
// --------------------------------------------------------
void populateConnections(struct graph* graph){
// Rooms directory is entered.
    chdir(newestDirName);
    int result = 0;
    char fileName[256];
    FILE *fp;
    int c;
    int b;

// For loop is initialized opening each file in the room directory
    for(b = 0; b < 7; b++){
        fp = fopen(graph->map[b].name,"r");
        if (fp == NULL){
            printf("An Error Occured");
        }

// Each line in the file is acquired.
// The first 10 characters are then compared to "CONNECTION"
// If a match is found, the strtok is used to split the line into an Array
        while(fgets(fileName, sizeof(fileName),fp) != NULL) {
            result = (strncmp(fileName, "CONNECTION", 10));
            if(result == 0){
                char* token = strtok(fileName, " ");
                int tokenNum = 0;

// The last word of the array or line is taken in as "token" variable
                while (token != NULL){
                    tokenNum++;
                    int resultCmp;
                    if(tokenNum == 3){

// The new line characer is removed from the token variable
                        token[strcspn(token,"\n")] = 0;
                        int x;

// Token is then compated to every room name in the graph structure for a match
// If a match is made, the rooms are connected int eh graph structure
                        for(x = 0; x < 7; x++){
                            resultCmp = strcmp(token, graph->map[x].name);
                            if(resultCmp == 0){
                                ConnectRoom(&graph->map[b], &graph->map[x]);
                            }
                        }
                    }
                    token = strtok(NULL, " ");
                }
            }
        }
        fclose(fp);         // file is closed
    }
    chdir("..");            // Directory is exited
}

// --------------------------------------------------------
//  THREADING FUNCTION IS ESTABLISHED
//  The function creates a file named currentTime with the fopen function
//  The time is taken using the time and local time functions
//  The time is formatted appropriately using the strftime function
//  The time is then printed into the file before it is closed.
//  The function is then opened again to display into the console
//  The file is then closed
//  This allows the function to be rewritten with new times
//  NULL is returned
// --------------------------------------------------------
void *multiThreading(void *value){
    FILE* timeFile;
    FILE* timeFileRead;
    char character;

// currentTime.txt file is opened with write+ attribute
// Local time is identified
    timeFile = fopen("currentTime.txt", "w+");
    time_t timeCalc;
    struct tm *timeStruct;
    char buffer[100];
    time( &timeCalc );
    timeStruct = localtime( &timeCalc );

// The time is formatted appropriately
// Date is written into file
// File is closed
    strftime(buffer, 100, "%I:%M%p, %A, %B %d, %Y", timeStruct);
    fprintf(timeFile,"%s\n",buffer);
    fclose(timeFile);

// File is reopened to be read with r attribute
// While loop prints our characters of file to console
    timeFileRead = fopen("currentTime.txt", "r");
    character = fgetc(timeFileRead);
    while (character != EOF)
   {
       printf ("%c", character);
       character = fgetc(timeFileRead);
   }

// File is closed
   fclose(timeFileRead);
	return NULL;
}


// --------------------------------------------------------
//  STARTGAME FUNCTION IS ESTABLISHED
//  The game starts by finding the START_ROOM and identified as CurrentRoom
//  The the start room is added to the first position in the roomsTraversed array
//  THE GAME BEGINS:
//          The connections are displayed
//          User selects the room to enter
//          Room is added to roomsTraversed Array
//          IF the user prints "time" the time is displayed using multithreading
//          IF the END_ROOM is found, the game ends.
// --------------------------------------------------------
void startGame(struct graph* graph){
    struct room CurrentRoom;                // Current Room Room struct is established
    char *nameBuffer;                       // nameBuffer is declared for user input
    size_t bufsize = 32;
    size_t characters;
    nameBuffer = (char *)malloc(bufsize * sizeof(char));
    int gameFinish = 1;
    int i;

// Initial room is identified within the structure and defined as CurrentRoom
// Current room is added to first roomsTraversed position in array
    for(i = 0; i < 7; i++){
        if (graph->map[i].type == 1){
            CurrentRoom = graph->map[i];
            roomsTraversed[0] = graph->map[i].name;
        }
    }
    int steps = 0;      // steps variable keeps track of the number of rooms traversed

// GAME BEGINS!!!!!
    while(gameFinish){
        int userInput = 1;
        int result = 1;
// Current Room is compared to END_ROOM
// If the END_ROOM is entered, the program ends, and the appropriate output is displayed
// Rooms traversed and the number of steps taken are declared
        if(CurrentRoom.type == 3){
            printf("YOU HAVE FOUND THE END OF THE ROOM. CONGRATULATIONS!\n");
            printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", steps);
            int s;
            for(s = 0; s < steps; s++){
                printf("%s\n", roomsTraversed[s]);
            }
            gameFinish = 0;
            break;
        }

// User input while loop is initialized
// Current Location and the possible connections are displayed to user
        while (userInput){
            printf("CURRENT LOCATION: %s\n", CurrentRoom.name);
            printf("POSSIBLE CONNECTIONS: ");

// Room connections are displayed
            int r;
            for(r = 0; r < CurrentRoom.numConnections - 1; r++){
                printf("%s, ", CurrentRoom.connections[r]->name);
            }
            printf("%s.\n", CurrentRoom.connections[CurrentRoom.numConnections - 1]->name);

// UserInput is taken in as nameBuffer
            printf("WHERE TO? > ");
            int foundVal = 0;
            characters = getline(&nameBuffer, &bufsize, stdin);

// Newline character is removed from userInput
            nameBuffer[strcspn(nameBuffer,"\n")] = 0;
            printf("\n");
            int x;

// UserInput is compared to the available connections
// If a match is found, the room is enterd
// Found room is declared as CurrentRoom
// new room is added to roomsTraversed
// steps int variable is incremented
            for(x = 0; x < CurrentRoom.numConnections; x++){
                result = strcmp(nameBuffer, CurrentRoom.connections[x]->name);
                if(result == 0){
                    steps = steps + 1;
                    foundVal = 1;
                    result = 1;
                    int y;
                    for(y = 0; y < 7; y++){
                        if (strcmp(graph->map[y].name, CurrentRoom.connections[x]->name) == 0){
                            CurrentRoom = graph->map[y];
                            roomsTraversed[steps-1] = graph->map[y].name;
                            userInput = 0;
                            break;
                        }
                    }
                }
            }

// If the user types the word time, the program displayd the current time using multithreading
            if (strcmp(nameBuffer, "time") == 0){
                pthread_t thread;
                pthread_mutex_init(&mutex, NULL);
                pthread_mutex_lock(&mutex);
                int multiThreadID = pthread_create(&thread, NULL, multiThreading, NULL);
                pthread_mutex_unlock(&mutex);
                pthread_mutex_destroy(&mutex);
                sleep(1);
                break;
            }

// If the userInput is not found in available rooms, the appropriate prompt is displayed.
            if (foundVal == 0){
                printf("HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
                break;
            }
        }
        gameFinish = 1;
    }
}

// --------------------------------------------------------
//  MAIN IS ESTABLISHED
// --------------------------------------------------------
int main(){
    FindDirectory();                // Appropriate directory is identified
    struct graph graph;             // Graph structure is created
    graph.numRooms = 0;
    genGraph(&graph);               // Graph rooms are identified and created
    populateFiles(&graph);          // Graph room types are identified and alloated
    populateConnections(&graph);    // Graph room connections are identified and allocated.


/* USED TO VIEW START AND END BEFORE START. USED FOR CHECKING PROGRAM
    int i;
    for(i = 0; i < 7; i++){
    printf("Room Name: %s TYPE: %d \n", graph.map[i].name, graph.map[i].type);
    }
    printf("\n\n\n");
*/


    startGame(&graph);              // Room begins
 return 0;
}
