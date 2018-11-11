//Yilun Jiang
//Othello AI
//Prof. Sable

#include <iostream>
#include <string>
#include <ctime>
using namespace std;

void display_board();
void legal_move(int curr_board [64], int player, int * player_choice);
void * detect (int currPos, int direction, int curr_board [64], int * futurePieces);
void display_legal_move(int player_choice[32]);
void display_one_move(int index);
void set_board();
void move(int curr_board[64], int nextPos, int player);
bool detect_endGame();
int alpha_beta_pruning(int depth, int player);
int max_value(int alpha, int beta, int curr_board[64], int depth, int * addressOfPos, int depthOfFirstLevel);
int min_value(int alpha, int beta, int curr_board[64], int depth, int * addressOfPos, int depthOfFirstLevel);
int evaluateState(int curr_board[64], int player);
int countPiece(int curr_board[64], int player);
void wrong_input(); //deals with the wrong input
//white piece is indicated by 1
//black piece is indicated by -1
int board [64] = { 
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 1, -1, 0, 0, 0,
                   0, 0, 0, -1, 1, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0,            
                  };

//The weight I decided for each cell. It is one of the factors used to evaluate the value of a node in the alpha-beta pruning
//Assign high value to corners and negative values as penalty for entering some of the undesired places
const int weightedScore [64] = { 
                                120, -20,  20,   5,   5,  20, -20, 120,
                                -20, -40,  -5,  -5,  -5,  -5, -40, -20,
                                20,  -5,  15,   3,   3,  15,  -5,  20,
                                5,  -5,   3,   3,   3,   3,  -5,   5,
                                5,  -5,   3,   3,   3,   3,  -5,   5,
                                20,  -5,  15,   3,   3,  15,  -5,  20,
                                -20, -40,  -5,  -5,  -5,  -5, -40, -20,
                                120, -20,  20,   5,   5,  20, -20, 120
                                };

double time_limit = 5; //The default time limit for one move for AI
int user_side = -1; //The default side for user is black(-1)
int mode = 1; //The default mode for the game is start fresh
time_t start; //setting the time of the start of search as a global variable so that in the recursive alpha-beta pruning, they can still acess it and determine if they need exit immediately. 
int main()
{
    cout << "Welcome to Othello!" << endl;
    cout << "Please input a time limit for the AI to use in one move. Just one number. Don't input unit. The default unit is sec." << endl;
    //Protect against bad inputs
    while(!(cin >> time_limit))
    {
        wrong_input();
        cout << "Wrong input. Just put the number. Don't put unit. The default unit is sec." << endl;
    }
    time_limit = time_limit - 0.2; //We will end our search 0.2 secs before the requested time.
    cout << "Type 1 if you want to start a normal game, 2 if you want to start with a specific board and side and 3 if you want the computer to play agaisnt itself." << endl;
    //Protect against bad inputs
    while(!(cin >> mode))
    {
        wrong_input();
        cout << "Wrong input. Type 1 if you want to start a normal game, 2 if you want to start with a specific board and side and 3 if you want the computer to play agaisnt itself." << endl;
    }

    //Set the board
    if(mode == 2)
        set_board();
    
    int player_choice [32]; //player_choice is an array recording the linear index for cells that are legal to play
    //player's choice for the next legal move based on the prompt given by the program. It serves as an index into the player_choice array. The default is the first available one;
    int choice = 0; 

    if (mode != 3)
    {
        cout << "Please choose white or black. 1 means white and -1 means black. Note black always take the first move." << endl;
        cout << "If you want to play first, pick black(-1) and if you want AI to make first move, pick white(1)." << endl;
        //Protect against bad inputs
        while(!(cin >> user_side) || user_side < -1 || user_side > 1 || user_side == 0)
        {
            wrong_input();
            cout << "Wrong input. Please choose white or black. 1 means white and -1 means black. Just put the number." << endl;
        }

        while(!detect_endGame())//return false when the game can still be played and true when the game ends
        {
            if(user_side == 1)//Player is white, computer is black
            {

                legal_move(board, -1, player_choice);

                if(player_choice[0] == -1) //AI has no possible moves
                {
                    cout << "No legal moves available! Skip AI's turn." << endl;             
                }
                else
                {
                    //Computer makes a move, start of search
                    start = time(0); //get time for the start of search
                    int depth = 1;//We start the search with going down only one level
                    int pos = 0;
                    cout << "Start of AI search." << endl;
                    while(1)
                    {
                        int currentPos = alpha_beta_pruning(depth, -1);
                        //if the return of alpha_beta_pruning isn't -99, it means we completed the search in the current level. So we update our result.
                        if(currentPos != -99)
                            pos = currentPos;
                        else //We broke out of the search without finishing the current level, so we use result from the previous level.
                            break;
                        depth++; //There is time left, we can go search one level deeper
                    }
                    time_t end = time(0);
                    double elapsed = difftime(end, start);
                    cout << "AI used " << elapsed << " secs." << endl;
                    cout << "MAximum depth has reached is " << depth << endl;
                    cout << "AI takes a move -------> ";
                    display_one_move(pos);
                    cout << endl;
                    move(board, pos, -1);
                    cout << endl;
                    cout << "Board after AI move." << endl;                
                }
            }

            //Player makes a move
            display_board();
            legal_move(board, user_side, player_choice);
            display_legal_move(player_choice);

            //Player doesn't have a legal move, skip turn.
            if(player_choice[0] == -1)
            {
                cout << "No legal moves available! Skip your turn." << endl;
            }
            //Protect against bad inputs
            else
            {
                while(!(cin >> choice) || choice < 1 || choice > 32 || player_choice[choice-1] == -1)
                {
                    wrong_input();
                    cout << "Bad inputs. Please type in a valid number. Each move is numbered. Just put in the number in front of the option." << endl;
                }

                move(board, player_choice[choice-1], user_side);
                cout << endl;
                cout << "Board after your move." << endl;
                display_board();
            }

            

            if(user_side == -1)//Player is black and computer is white(1)
            {
                legal_move(board, 1, player_choice);

                if(player_choice[0] == -1) //AI has no possible moves
                {
                    cout << "No legal moves available! Skip AI's turn." << endl;              
                }
                else
                {
                    
                    start = time(0); //get time for the start of search
                    int depth = 1;//We start the search with going down only one level
                    int pos = 0;
                    cout << "Start of AI search." << endl;
                    while(1)
                    {
                        int currentPos = alpha_beta_pruning(depth, 1);
                        //if the return of alpha_beta_pruning isn't -99, it means we completed the search in the current level. So we update our result.
                        if(currentPos != -99)
                            pos = currentPos;
                        else //We broke out of the search without finishing the current level, so we use result from the previous level.
                            break;
                        depth++; //There is time left, we can go search one level deeper
                    }
                    time_t end = time(0);
                    double elapsed = difftime(end, start);
                    cout << "AI used " << elapsed << " secs." << endl;
                    cout << "MAximum depth has reached is " << depth << endl;
                    cout << "AI takes a move -------> ";
                    display_one_move(pos);
                    cout << endl;
                    move(board, pos, 1);
                    cout << endl;
                    cout << "Board after AI move." << endl;    
                }            
            }            
        }
        
        cout << "Game ends!" << endl;
        display_board();
        cout << endl;

        int user_piece = countPiece(board, user_side);
        if(user_piece == 32)//It's a draw
        {
            cout << "*****************************************************************************************" << endl;
            cout << "*                                      DRAW                                             *" << endl;
            cout << "*****************************************************************************************" << endl;
            cout << "User has 32 pieces." << endl;
            cout << "AI has 32 pieces." << endl; 
        }
        else if(user_piece > 32)//Player wins
        {
            cout << "*****************************************************************************************" << endl;
            cout << "*                                  PLAYER WINS                                          *" << endl;
            cout << "*****************************************************************************************" << endl;
            cout << "User has " << user_piece << " pieces." << endl;
            cout << "AI has " << 64 - user_piece << " pieces." << endl;        
        }
        else
        {
            cout << "*****************************************************************************************" << endl;
            cout << "*                                    AI WINS                                            *" << endl;
            cout << "*****************************************************************************************" << endl;
            cout << "User has " << user_piece << " pieces." << endl;
            cout << "AI has " << 64 - user_piece << " pieces." << endl;  
        }        
    }
    else//Computer plays against itself
    {
        cout << "AI-alpha is black." << endl;
        cout << "AI-beta is white." << endl;
        display_board();
        while(!detect_endGame())
        {   
            legal_move(board, -1, player_choice);
            display_legal_move(player_choice);

            if(player_choice[0] == -1) //AI has no possible moves
            {
                cout << "No legal moves available! Skip AI-alpha's turn." << endl;             
            }
            else
            {
                //Computer makes a move, start of search
                start = time(0); //get time for the start of search
                int depth = 1;//We start the search with going down only one level
                int pos = 0;
                while(1)
                {
                    int currentPos = alpha_beta_pruning(depth, -1);
                    //if the return of alpha_beta_pruning isn't -99, it means we completed the search in the current level. So we update our result.
                    if(currentPos != -99)
                        pos = currentPos;
                    else //We broke out of the search without finishing the current level, so we use result from the previous level.
                        break;
                    depth++; //There is time left, we can go search one level deeper
                }
                time_t end = time(0);
                double elapsed = difftime(end, start);
                cout << "Black used " << elapsed << " secs." << endl;
                cout << "MAximum depth has reached is " << depth << endl;
                cout << "Black takes a move -------> ";
                display_one_move(pos);
                cout << endl;
                move(board, pos, -1);
                cout << endl;
                cout << "Board after Black move." << endl;                
            }
            display_board();
            
            if(detect_endGame())
                break;
            int beta;
            cout << "Type 1 to let white to move." << endl;

            while(!(cin >> beta) || beta != 1)
            {
                wrong_input();
                cout << "Wrong input. Type 1 to let white to move." << endl;
            }

            
            legal_move(board, 1, player_choice);
            display_legal_move(player_choice);

            if(player_choice[0] == -1) //AI has no possible moves
            {
                cout << "No legal moves available! Skip white's turn." << endl;             
            }
            else
            {
                //Computer makes a move, start of search
                start = time(0); //get time for the start of search
                int depth = 1;//We start the search with going down only one level
                int pos = 0;
                while(1)
                {
                    int currentPos = alpha_beta_pruning(depth, 1);
                    //if the return of alpha_beta_pruning isn't -99, it means we completed the search in the current level. So we update our result.
                    if(currentPos != -99)
                        pos = currentPos;
                    else //We broke out of the search without finishing the current level, so we use result from the previous level.
                        break;
                    depth++; //There is time left, we can go search one level deeper
                }
                time_t end = time(0);
                double elapsed = difftime(end, start);
                cout << "White used " << elapsed << " secs." << endl;
                cout << "MAximum depth has reached is " << depth << endl;
                cout << "White takes a move -------> ";
                display_one_move(pos);
                cout << endl;
                move(board, pos, 1);
                cout << endl;
                cout << "Board after White's move." << endl;
                display_board();                
            }

            if(detect_endGame())
                break;

            cout << "Type 1 to let Black move." << endl;

            while(!(cin >> beta) || beta != 1)
            {
                wrong_input();
                cout << "Wrong input. Type 1 to let Black move." << endl;
            }   
        }
    
        cout << "Game ends!" << endl;
        display_board();
        cout << endl;

        int black_piece = countPiece(board, -1);
        if(black_piece == 32)//It's a draw
        {
            cout << "*****************************************************************************************" << endl;
            cout << "*                                      DRAW                                             *" << endl;
            cout << "*****************************************************************************************" << endl;
            cout << "Black has 32 pieces." << endl;
            cout << "White has 32 pieces." << endl; 
        }
        else if(black_piece > 32)//Player wins
        {
            cout << "*****************************************************************************************" << endl;
            cout << "*                                   BLACK WINS                                          *" << endl;
            cout << "*****************************************************************************************" << endl;
            cout << "Black has " << black_piece << " pieces." << endl;
            cout << "White has " << 64 - black_piece << " pieces." << endl;        
        }
        else
        {
            cout << "*****************************************************************************************" << endl;
            cout << "*                                   WHITE WINS                                          *" << endl;
            cout << "*****************************************************************************************" << endl;
            cout << "Black has " << black_piece << " pieces." << endl;
            cout << "White has " << 64 - black_piece << " pieces." << endl;  
        }
    }
    

}

void display_board()
{
    string background = "\033[42;30m";
    string reset = "\033[0m";
    string white = "\033[42;37m";
    string black = "\033[42;30m";
    cout << background << "    a    b    c    d    e    f    g    h    " << reset << endl;
    cout << background << "  ========================================= " << reset << endl;
    for(int row = 0; row < 8; row++)
    { 
        cout << background << " ||   ||   ||   ||   ||   ||   ||   ||   || " << reset << endl;
        cout << background << row+1 << "||" << reset;
        for(int col = 0; col < 8; col++)
        {
            if(board[row*8+col] == 0)
                cout << background << "   ||" << reset;
            else if(board[row*8+col] == 1) //print white piece
                cout << white << " \u25A0 " << reset << background << "||" << reset;
            else
                cout << black << " \u25A0 ||" << reset; //print black piece
            if(col == 7)
                cout << background << row+1 << reset << endl;
        }
        cout << background << " ||   ||   ||   ||   ||   ||   ||   ||   || " << reset << endl;
        cout << background << "  ========================================= " << reset << endl;
    }
    cout << background << "    a    b    c    d    e    f    g    h    " << reset << endl;
}

//Given the current board diagram, player in this turn and a pointer to the player_choice array
//It places the lienar index for the possible next move in the player_choice array
void legal_move(int curr_board [64], int player, int * player_choice) //player is 1 equals white and -1 equals black
{   
    //Keep track of how many legal moves have been found and use it to index the player_choice array to store the linear index of the legal cells
    int count = 0; 
    for(int i = 0; i < 32; i++)
        player_choice[i] = -1;
    for(int row = 0; row < 8; row++)
    {
        for(int col = 0; col < 8; col++)
        {
            if(curr_board[row*8+col] == 0) //Only check for positions that haven't been occupied
            {
                for(int dir = 1; dir < 9; dir++) //check for 8 total possible direction
                {
                    int futurePieces [7]= {0,0,0,0,0,0,0};
                    detect(row*8+col, dir, curr_board, futurePieces); //The detect function will give all the possible cells in the given direction and the given pos.
                    //If the next piece in the given direction is the opponent's piece, we have hope that this direction might 
                    //prove the cell we are checking is a legal cell to play on 
                    if(*futurePieces == -1*player) 
                    {
                        //if we can find a piece belongs to us from futurePieces[1~6], then this cell will be a legal cell to play on
                        for(int i = 1; i < 7; i++)
                        {
                            //if we hit a blank cell before we hit one of our pieces, then we know this cell can't be a legal cell to play on
                            if(futurePieces[i] == 0)
                                break;
                            else if(futurePieces[i] == player)
                            {
                                //cout << "player_choice index " << count << "is confirmed to be a legal cell"
                                //     << " through dir " << dir << endl;
                                player_choice[count] = row*8+col;
                                count += 1;
                                break;
                            }    
                        }
                        //To save the time to explore other directions if we already know from one direction, it is a legal cell to play on
                        if(player_choice[count-1] == row*8+col)
                            break;                             
                    }      
                }
            }   
        }
    }
}

//Given the board diagram, current postion, direction and a pointer to an int array,
//output all cells in that direction. 1 indicates white piece, -1 indicates black piece
//0 represents no piece on that cell
void * detect(int currPos, int direction, int curr_board [64], int * futurePieces)
{
    int i = 0; //Used to index the futurePieces
    switch(direction)
    {
        case 1: //straight up
            while(currPos-8 > -1)
            {
                futurePieces[i] = curr_board[currPos - 8];
                ++i;
                currPos -= 8;
            }
                
        break;
        case 2: //up right direction
            while(currPos-7 > -1 && currPos % 8 != 7)
            {
                futurePieces[i] = curr_board[currPos - 7];
                ++i;
                currPos -= 7;  
            }
        break;
        case 3: //right direction
            while(currPos+1 < 64 && currPos % 8 != 7)
            {
                futurePieces[i] = curr_board[currPos + 1];
                ++i;
                currPos += 1;
            }
        break;
        case 4: //right down direction
            while(currPos+9 < 64 && currPos % 8 != 7)
            {
                futurePieces[i] = curr_board[currPos + 9];
                ++i;
                currPos += 9;
            }
        break;
        case 5: //down direction
            while(currPos+8 < 64)
            {
                futurePieces[i] = curr_board[currPos + 8];
                ++i;
                currPos += 8;
            }
        break;   
        case 6: //down left direction
            while(currPos+7 < 64 && currPos % 8 != 0)
            {
                futurePieces[i] = curr_board[currPos + 7];
                ++i;
                currPos += 7;
            } 
        break;     
        case 7: //left direction
            while(currPos-1 > -1 && currPos % 8 != 0)
            {
                futurePieces[i] = curr_board[currPos - 1];
                ++i;
                currPos -= 1;
            }
        break;
        case 8: //left up direction
            while(currPos-9 > -1 && currPos % 8 != 0)
            {
                futurePieces[i] = curr_board[currPos - 9];
                ++i;
                currPos -= 9;
            }
        break;
    }
}

//Given the player_choice array, display to the screen what's a legal cell to play on
void display_legal_move(int player_choice[32])
{
    int count = 1;
    cout << "The following are legal moves. Each move is numbered. Please put in the number in front of the option." << endl;
    for(int i = 0; i < 32; i++)
    {
        if(player_choice[i] == -1) //No further leagal moves
            break;
        cout << count << ".";
        display_one_move(player_choice[i]);
        count++;
    }
    cout << endl;
}

void display_one_move(int index)
{
    int row = index / 8;
    int col = index % 8;
        
    switch(col)
    {
        case 0:
            cout << 'a';
        break;
        case 1:
            cout << 'b';
        break;
        case 2:
            cout << 'c';
        break;
        case 3:
            cout << 'd';
        break;
        case 4:
            cout << 'e';
        break;
        case 5:
            cout << 'f';
        break;
        case 6:
            cout << 'g';
        break;
        case 7:
            cout << 'h';
        break;
    }
    cout << row+1 << "  ";
}

//The function is used to set the game with a specific given board state and a side from the user's input
//It returns side to the main function
void set_board()
{
    cout << "Please input the desired board shape. Using 1 to represent white, -1 representing black and 0 representing blank cell." << endl;
    cout << "Insert a space or an enter between two numbers. Ex: 1 -1 0 0 1 -1 etc. There should be a total of 64 numbers." << endl;
    cout << "For convenience, you can also separate 64 numbers into 8 lines." << endl;
    cout << "Sample input: " << endl;
    cout << "0 0 1 1 1 1 0 0" << endl;
    cout << "0 0 1 1 1 0 0 0" << endl;
    cout << "0 1 -1 -1 -1 -1 0 0" << endl;
    cout << "1 1 1 1 -1 -1 1 1" << endl;
    cout << "1 1 1 1 1 1 1 1" << endl;
    cout << "1 0 1 1 1 1 0 0" << endl;
    cout << "0 0 0 1 0 0 0 0" << endl;
    cout << "0 0 0 0 0 0 0 0\n" << endl;
    cout << "Your input:" << endl;
    int curr_board [64];
    for(int i = 0; i < 64; i++)
    {
        //Protect against bad inputs
        while(!(cin >> curr_board[i]) || curr_board[i] < -1 || curr_board[i] > 1)
        {
            wrong_input();
            cout << "Wrong input. Please try again! Start from beginning. Using 1 to represent white, -1 representing black and 0 representing blank cell." << endl;     //error handling
            cout << "Sample input: " << endl;
            cout << "0 0 1 1 1 1 0 0" << endl;
            cout << "0 0 1 1 1 0 0 0" << endl;
            cout << "0 1 -1 -1 -1 -1 0 0" << endl;
            cout << "1 1 1 1 -1 -1 1 1" << endl;
            cout << "1 1 1 1 1 1 1 1" << endl;
            cout << "1 0 1 1 1 1 0 0" << endl;
            cout << "0 0 0 1 0 0 0 0" << endl;
            cout << "0 0 0 0 0 0 0 0\n" << endl;
            cout << "Your input:" << endl;
        }       
    }
    for(int i = 0; i < 64; i++)
        board[i] = curr_board[i];
    
}

//Given the nextPos in the 64 elements long array board and the player
//move function put player's piece onto the corresponding cell and flip other possible cells acrroding to the rule
void move(int curr_board[64], int nextPos, int player)
{
    curr_board[nextPos] = player;
    for(int dir = 1; dir < 9; dir++)
    {
        int futurePieces [7]= {0,0,0,0,0,0,0};
        detect(nextPos, dir, curr_board, futurePieces);
        int flipCount = 0;
        //If the adjacent cell in the direction is opppnent's piece
        //then we need to check if we need to flip along this diretion
        if(futurePieces[0] == -1*player)
        {
            ++flipCount;
            for(int i = 1; i < 7; i++)
            {
                //if we hit a blank cell before we hit one of our pieces, then we know we don't need to flip along this direction
                if(futurePieces[i] == 0)
                    break;
                //As soon as we hit one of our pieces, we stop searching along this direction and flip the pieces between the
                //starting pos and the one we just found
                else if(futurePieces[i] == player)
                {
                    //cout << "which index of futurePieces has one piece belongs to us. " << i << endl;
                    int step = 0; //Linear index increment/decrement for advancing along the direction
                    switch(dir)
                    {
                        case 1: step = -8; break;
                        case 2: step = -7; break;
                        case 3: step = 1; break;
                        case 4: step = 9; break;
                        case 5: step = 8; break;
                        case 6: step = 7; break;
                        case 7: step = -1; break;
                        case 8: step = -9; break;
                    }
                    int totalPos = nextPos; //totalPos is the linear index for pieces that need to be flipped next
                    //cout << "Flipcount is " << flipCount << endl;
                    for(int i = 1; i < flipCount+1; i++)
                    {
                        totalPos += step;
                        curr_board[totalPos] = player;
                    }
                }
                else //It is an opponent's piece and we add one to the flipCount
                    ++flipCount;    
            }
        }
        //cout << "flipCount for dir " << dir << "is " << flipCount << endl;
    }
}

//If there is a piece in every possible position, then the game ends
//return false when the game can still be played and true when the game ends
bool detect_endGame()
{
    for(int row = 0; row < 8; row++)
    {
        for(int col = 0; col < 8; col++)
        {
            if(board[row*8+col] == 0)
                return false;
        }
    }
    return true;
}

//Depth is the depth limit that we can go and the parameter player indicates whether we should go to a max node or a min node during the beginning of the search
int alpha_beta_pruning(int depth, int player)
{
    int alpha = -9999; //The highest value we have found so far along the path for max
    int beta = 9999; //the lowest value we have found so far along the path for min
    int curr_board[64]; //Given the current board for the alpha-beta pruning to search
    for(int i = 0; i < 64; i++)
        curr_board[i] = board[i];
    int player_choice[32]; //At each level, it needs to create an array recording what are the possible moves
    legal_move(curr_board, player, player_choice);
    int pos = player_choice[0]; //The best cell to be played on is assumed to the first legal cell
    int * addressOfPos = &pos;
    int max = 0;
    int min = 0;
    if(player == 1)
        max = max_value(alpha, beta, curr_board, depth, addressOfPos, depth);
    else
        min = min_value(alpha, beta, curr_board, depth, addressOfPos, depth);

    if(max == -9999 || min == -9999) //The program ends searching during the middle of a level. Return the control back to main fucntion and indicate the situation with -99
        return -99;
    return pos;
}

//For alpha-beta pruning targeted at different depth, it needs to return which cell is the best one to move to the main function
//In order to achieve this, I use a pointer *addressOfPos and an int parameter depthOfFirstLevel. When a max node at the same height as the
//given targeted depth get its value, it knows that's the pos it should report back to main fucntion. The rest is just standard minimax-search
int max_value(int alpha, int beta, int curr_board[64], int depth, int * addressOfPos, int depthOfFirstLevel) //used to calculate white move
{
    --depth;
    int nextLevel = 0;
    int bestNextLevel = -9999;
    int player_choice[32]; //At each level, it needs to create an array recording what are the possible moves
    legal_move(curr_board, 1, player_choice);
    //If time allows to go further deep down, then it keeps going.

    //No legal moves for AI
    if(player_choice[0] == -1)
        return -9999;
    if(depth != 0)
    {
        //Variable numMovesExplored is used to keep track of how many legal moves have been explored at this level
        for(int numMovesExplored = 0; numMovesExplored < 32 && player_choice[numMovesExplored] != -1; numMovesExplored++)
        //It needs to creat a new board everytime we choose a possible move and go down one level since it needs to test multiple moves
        {
            time_t end = time(0);
            double elapsed = difftime(end, start);
            //cout << "Time elapsed at a max node is " << elapsed << endl;
            if(elapsed > time_limit) //The program has used up its time, end search right now
                return -9999;
            int curr_board1[64]; 
            for(int i = 0; i < 64; i++)
                curr_board1[i] = curr_board[i];
            move(curr_board1, player_choice[numMovesExplored], 1);

//*********************************************************************************************************************************************************************************
            //If the search has found an end game position, then what matter's is only how many pieces are on the board
            if(detect_endGame())
                return 1000*countPiece(curr_board1,1);
//******************************************************************************************************************************************************************************8*****
            nextLevel = min_value(alpha, beta, curr_board1, depth, addressOfPos, depthOfFirstLevel);
            if(nextLevel == -9999) //return up towards the top of recursion
                return -9999;
            else if(nextLevel > bestNextLevel)
            {
                bestNextLevel = nextLevel;
                if(depth == depthOfFirstLevel - 1)
                    * addressOfPos = player_choice[numMovesExplored];
            }
            if(nextLevel >= beta)
            {
                //cout << "Pruning happenes at a max node." << endl;
                break;
            }
            else if(alpha < nextLevel)
                alpha = nextLevel;               
        }
    } 
    else //The allowed depth is zero now, it needs to use heuristic to estimate how good a state is
    {
        return evaluateState(curr_board, 1);//This return is used for leaves
    }
    return bestNextLevel;//If this state is an intermiediate state, then it returns alpha. The biggest possible value it has found so far.
}

//Pos and depthOfFirstLevel are used to record which is the best cell to move to. 
int min_value(int alpha, int beta, int curr_board[64], int depth, int * addressOfPos, int depthOfFirstLevel) //used to caluclate black move
{
    --depth;
    int nextLevel = 0;
    int bestNextLevel = 9999;
    int player_choice[32]; //At each level, it needs to create an array recording what are our possible moves
    legal_move(curr_board, -1, player_choice);

    //No legal moves for AI
    if(player_choice[0] == -1)
        return 9999;
    //If time allows to go further deep down, then it keeps going.
    if(depth != 0 )
    {
        //Variable numMovesExplored is used to keep track of how many legal moves have been explored at this level
        for(int numMovesExplored = 0; numMovesExplored < 32 && player_choice[numMovesExplored] != -1; numMovesExplored++)
        //We need to creat a new board everytime we choose a possible move and go down one level since we need to test multiple moves
        {
            time_t end = time(0);
            double elapsed = difftime(end, start);
            //cout << "Time elasped at a min node is " << elapsed << endl;
            if(elapsed > time_limit) //We have used up our time, end search right now
                return -9999;
            int curr_board1[64]; 
            for(int i = 0; i < 64; i++)
                curr_board1[i] = curr_board[i];
            move(curr_board1, player_choice[numMovesExplored], -1);

//*********************************************************************************************************************************************************************************
            //If the search has found an end game position, then what matter's is only how many pieces are on the board
            if(detect_endGame())
                return 1000*countPiece(curr_board1,-1);
//******************************************************************************************************************************************************************************8*****
            nextLevel = max_value(alpha, beta, curr_board1, depth, addressOfPos, depthOfFirstLevel);
            if(nextLevel == -9999) //return up towards the top of recursion
                return -9999;
            else if(nextLevel < bestNextLevel)
            {
                bestNextLevel = nextLevel;
                if(depth == depthOfFirstLevel - 1)
                    * addressOfPos = player_choice[numMovesExplored];
            }
            if(nextLevel <= alpha)
            {
                //cout << "Pruning happenes at a min node." << endl;
                break;
            }
            else if(beta > nextLevel)
                beta = nextLevel;               
        }
    } 
    else //The allowed depth is zero now, we need to use heuristic to estimate how good a state is
    {
        return evaluateState(curr_board, -1); //This return is used for leaves
    }
    return bestNextLevel;//If this state is an intermiediate state, then we return beta. The smallest possible value we have
}


int evaluateState(int curr_board[64], int player)
{
    int player_choice [32];
    legal_move(curr_board, player, player_choice);
    int count = 0; //To count how many legal moves are available for the player at the current board position
    for(int i = 0; i < 32; i++)
    {
        if(player_choice[i] != -1)
            ++count;
        else
            break;
    }
    int value = 0; //To compute the board value, assigning high values to corners and sides. Negative values for some of the bad positions
    for(int i = 0; i < 64; i++)
        value += curr_board[i] * weightedScore[i];

    int estimate = 10 * player * count + value; //This is my heuristic function. It is a linear combination of mobility(possible moves) and board values
    return estimate;
}

//It is used to evaluate the end game state. What matters here is only how many pieces are on the board
int countPiece(int curr_board[64], int player)
{
    int count = 0;
    for(int i = 0; i < 64; i++)
    {
        if(curr_board[i] == player)
            ++count;
    }
    return count;
}

void wrong_input() //deals with the wrong input
{
    cin.clear();
    cin.ignore(1000, '\n');
}