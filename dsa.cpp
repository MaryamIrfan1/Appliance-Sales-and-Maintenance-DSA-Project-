#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <climits>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <mysql.h> //for database
#include <conio.h>
#include <sstream>

#define V 11
#define V1 6

using namespace std;

HINSTANCE hInst; // Instance handle of the application
string loggedInUser; // To store logged in user

//DATABASE CONFIGURATIONS
MYSQL* conn;

void connectToDatabase() {
    conn = mysql_init(NULL);
    if (!conn) {
        MessageBox(NULL, "MySQL initialization failed!", "Error", MB_OK | MB_ICONERROR);
        exit(1);
    }

    if (!mysql_real_connect(conn, "localhost", "root", "", "ApplianceShop", 3306, NULL, 0)) {
        MessageBox(NULL, "Connection to MySQL database failed!", "Error", MB_OK | MB_ICONERROR);
        exit(1);
    }
}
void executeQuery(const string& query) {
    if (mysql_query(conn, query.c_str())) {
        MessageBox(NULL, mysql_error(conn), "Query Execution Failed", MB_OK | MB_ICONERROR);
    }
}
MYSQL_RES* fetchQueryResults(const string& query) {
    if (mysql_query(conn, query.c_str())) {
        cout << "Query Execution Failed: " << mysql_error(conn) << endl;
        return NULL;
    }
    return mysql_store_result(conn);
}
// Existing structures and functions are assumed to be included here
struct user {
    string name;
    string product;
    int quantity;
    double bill;

    user() {}

    user(string name, string product, int quantity, double bill) {
        this->name = name;
        this->product = product;
        this->quantity = quantity;
        this->bill = bill;
    }
};

struct deliveryUser {
    user nUser;
    string address;
    double deliveryCharges;
    int distanceDelivery;
    string userUsername;
    deliveryUser *next = NULL;

    deliveryUser(string name, string product, int quantity, double bill, string address, double deliveryCharges, int distanceDelivery, string userUsername) {
        this->nUser = user(name, product, quantity, bill);
        this->address = address;
        this->deliveryCharges = deliveryCharges;
        this->distanceDelivery = distanceDelivery;
        this->userUsername = userUsername;
    }
};

struct MaintenanceRequest {
    int requestId;
    string userName;
    string description;
    string reply;  // Field to store admin's reply
    MaintenanceRequest *next = nullptr;

    MaintenanceRequest(int id, string user, string desc) {
        requestId = id;
        userName = user;
        description = desc;
        reply = "";  // Initialize reply as an empty string
        next = nullptr;
    }
};

struct Shop {
    string name;
    vector<string> product;  // Vector for product names
    vector<int> price;       // Vector for product prices
    string address;
    deliveryUser* nextdeliveryUser = nullptr;

    // Constructor to initialize from database
    Shop() {
    	connectToDatabase();
        // Query to fetch products and their prices
        string query = "SELECT name, price FROM Products";
        if (mysql_query(conn, query.c_str())) {
            cout << "Failed to fetch products: " << mysql_error(conn) << endl;
            return;
        }

        MYSQL_RES* res = mysql_store_result(conn);
        if (res) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res))) {
                product.push_back(row[0]);  // Add product name
                price.push_back(stoi(row[1]));  // Add product price
            }
            mysql_free_result(res);
        } else {
            cout << "Failed to fetch result set: " << mysql_error(conn) << endl;
        }
    }
};


Shop* shop=NULL;
string addressF;
int dist[V];
int distanceP[V1];
int optionDelivery = -999;
int optionDelivery1 = -999;
deliveryUser *currentdeliveryUser = NULL;
MaintenanceRequest *maintenanceHead = nullptr; // Head pointer for the linked list of maintenance requests
int nextRequestId = 1;
int nextOrderId = 1;  // Global variable to keep track of the next order ID
string userUsername;

// Function declarations

struct takeAway
{
    string userUsername;
    string product;
    int quantity;
    double bill;
    int orderId;
    takeAway *left;
    takeAway *right;
    
    //takeAwayfromStore *next = NULL;

    takeAway(string userUsername, string product, int quantity, double bill, int orderId)
    {
        this->userUsername = userUsername;
		this->product = product;
        this->quantity = quantity;
        this->bill = bill;
        this->orderId=orderId;
        this->left = NULL;
        this->right = NULL;
    }
};

takeAway *root = NULL; 

void display(takeAway *root)
{
	cout <<"\n----------------------------------"<< endl;
//    cout << "Name :" << root->userUsername << endl;
    cout << "product :" << root->product << endl;
    cout << "Quantity : " << root->quantity << endl;
    cout << "Bill : " << root->bill << endl;
    cout << "Order ID: " << root->orderId << endl;
    cout <<"-----------------------------------\n"<< endl;
}

takeAway *displayTakeAway(takeAway *root)
{

    if (root)
    {
        displayTakeAway(root->left);
        display(root); 
        displayTakeAway(root->right);
    }

    return root;
}

// isEmpty or not
int isEmpty(takeAway *root)
{
    return root == NULL;
}


// Height of takeAway tree
int height(takeAway* root) {
	if (root == NULL)
      return -1;
    else {
      int leftH = height(root -> left);
      int rightH = height(root -> right);

    return (1+max(leftH , rightH));
	}
}

// Balance Factor for each takeAway node
int balanceFactor(takeAway *root)
{
    if (!root)
        return 0;

    return height(root->left) - height(root->right);
}

// Maximum of two integers as a helper function for height
int max(int a, int b)
{
    return (a > b) ? a : b;
}

// Searching in takeAway tree

takeAway *search(takeAway *root, int id)
{
    if (root == NULL)
    {
        return NULL;
    }
    else if (root->orderId == id)
    {
        return root;
    }
    else if (root->orderId < id)
    {
        return search(root->right, id);
    }
    else if (root->orderId > id)
    {
        return search(root->left, id);
    }

    return root;
}

// Balancing the takeAway's Tree thorugh AVL Rotations

// LL Rotation
takeAway *LLRotation(takeAway *root)
{
    takeAway *x = root->left;
    takeAway *temp = x->right;

    x->right = root;
    root->left = temp;

    root = x;

    return x;
}

// RR Rotation
takeAway *RRRotation(takeAway *root)
{

    takeAway *x = root->right;
    takeAway *temp = x->left;

    x->left = root;
    root->right = temp;

    root = x;

    return x;
}

// LR Rotation
takeAway *LRRotation(takeAway *root)
{
    root->left = RRRotation(root->left);
    return LLRotation(root);
}

// RL Rotation
takeAway *RLRotation(takeAway *root)
{
    root->right = LLRotation(root->right);
    return RRRotation(root);
}

//  INSERTION in takeAway Tree

takeAway *insertion(string name, int quantity, string product, double bill, int orderId, takeAway *root)
{
    takeAway *newNode = new takeAway(name,  product, quantity, bill, orderId);

    if (root == NULL)
    {
        root = newNode;
    }

    else if (root->orderId > newNode->orderId)
    {
        root->left = insertion(name, quantity, product, bill, orderId, root->left);
    }
    else if (root->orderId < newNode->orderId)
    {
        root->right = insertion(name, quantity, product, bill, orderId, root->right);
    }

    else
    {
        cout << "No duplicates Values are Allowed " << endl;
        return root;
    }

    int bf = balanceFactor(root);

    if (bf == 2)
    {
        // LL
        if (root->left->orderId > newNode->orderId)
        {
            return LLRotation(root);
        }

        // LR
        if (root->left->orderId < newNode->orderId)
        {
            return LRRotation(root);
        }
    }
    else if (bf == -2)
    {
        // RR
        if (root->right->orderId < newNode->orderId)
        {
            return RRRotation(root);
        }

        // RL
        if (root->right->orderId > newNode->orderId)
        {
            return RLRotation(root);
        }
    }

    return root; // in case there is no need of rotation
}

takeAway * minValue(takeAway *root) {
    takeAway *current = root;
    while (current-> left != NULL) {
      current = current -> left;
    }
    return current;
  }

takeAway *deleteNode(takeAway *root, int orderId)
{
    if (root == NULL)
        return root;
    else if (orderId < root->orderId)
        root->left = deleteNode(root->left, orderId);
    else if (orderId > root->orderId)
        root->right = deleteNode(root->right, orderId);
    else {
      if (root-> left == NULL) {
        takeAway* temp = root-> right;
        delete root;
        return temp;
      } 
	  else if (root-> right == NULL) {
        takeAway* temp = root-> left;
        delete root;
        return temp;
      } 
      
	  else {
        takeAway * temp = minValue(root-> right);
        root-> orderId = temp -> orderId;
        root-> right = deleteNode(root-> right, temp-> orderId );
      }
    }

    // getting the balance factor
    int balance = balanceFactor(root);

    // Rotation Cases
    // LEFT LEFT CASE
    if (balance > 1 && orderId < root->left->orderId)
        return LLRotation(root);

    // LEFT RIGHT CASE
    if (balance > 1 && orderId > root->left->orderId)
    {
        root->left = LLRotation(root->left);
        return LRRotation(root);
    }

    // RIHGT RIGHT CASE
    if (balance < -1 && orderId > root->right->orderId)
        return RRRotation(root);

    // RIGHT LEFT CASE
    if (balance < -1 && orderId < root->right->orderId)
    {
        return RLRotation(root);
    }

    return root;
}





//-----------------------------------------------------------------------------------------------------------------------
void placeOrderHomeDeliveryCustomer(string name, string product, int quantity, double bill, string address, int deliveryCharges, int distanceDelivery, string userUsername)
{
    currentdeliveryUser = new deliveryUser(name, product, quantity, bill, address, deliveryCharges, distanceDelivery, userUsername);

    if (shop->nextdeliveryUser == NULL)
    {
        shop->nextdeliveryUser = currentdeliveryUser;
    }
    else
    {
        deliveryUser *temp = shop->nextdeliveryUser;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }

        temp->next = currentdeliveryUser;
        currentdeliveryUser->next = NULL;
    }

    cout << "\n***************Order Details***************";
    cout << "\n Order name : " << product << "\n Quantity : " << quantity << "\n Total bill : " << bill;
    cout << "\n Address : " << address << "\n Distance in km : " << distanceDelivery << "\n Delivery charges : " << deliveryCharges << endl;
}


void displayAllOrdersHomeDeliveryCustomers()
{
    if (shop->nextdeliveryUser == NULL)
    {
        cout << "There is no Order for Home Delivery Customer till yet" << endl;
    }
    else
    {
        deliveryUser *traversal = shop->nextdeliveryUser;
        while (traversal != NULL)
        {
            cout << "-----------------------------------------------------" << endl;
//            cout << "Home Delivery Customer : " << traversal->userUsername << endl;
            cout << "Product Name : " << traversal->nUser.product << endl;
            cout << "Quantity : " << traversal->nUser.quantity << endl;
            cout << "Delivery Distance : " << traversal->distanceDelivery << "KM"<<endl;
            cout << "Delivery Charges : " << traversal->deliveryCharges << endl;
            cout << "Bill : " << traversal->nUser.bill << " RS/_" << endl;
            cout << "Delivery Address : " << traversal->address << endl;
            cout << "-----------------------------------------------------" << endl;

            traversal = traversal->next;
        }
    }
    
}





//-----------------------------------------------------------------------------------

int minDistance(int dist[], bool visited[])
{
    int min = INT_MAX, min_index;
    for (int v = 0; v < V; v++)
        if (visited[v] == false && dist[v] <= min)
            min = dist[v], min_index = v;
 
    return min_index;
}



void dijkstra(int graph[V][V], int dist[], int src)
{
    bool visited[V]; 

    for (int i = 0; i < V; i++)
        dist[i] = INT_MAX, visited[i] = false;

    dist[src] = 0;

    for (int count = 0; count < V - 1; count++) {
        int u = minDistance(dist, visited);

        visited[u] = true;
        
        for (int v = 0; v < V; v++)

            if (!visited[v] && graph[u][v] && dist[u] != INT_MAX && dist[u] + graph[u][v] < dist[v])
                	dist[v] = dist[u] + graph[u][v];
    }
}

//-----------------------------------------------------------------------------------

int minKey(int distanceP[], bool visitedP[])
{
	int min = INT_MAX, min_index;

	for (int v = 0; v < V1; v++)
		if (visitedP[v] == false && distanceP[v] < min)
			min = distanceP[v], min_index = v;

	return min_index;
}

void printMST(int parent[], int graph[V1][V1])
{
	cout << "Edge \tWeight\n";
	for (int i = 1; i < V1; i++)
		cout << parent[i] << " - " << i << " \t"
			<< graph[i][parent[i]] << " \n";
}

void prims(int graph[V1][V1], int distanceP[])
{
	int parent[V1];
	bool visitedP[V1];

	for (int i = 0; i < V1; i++)
		distanceP[i] = INT_MAX, visitedP[i] = false;

	distanceP[0] = 0;
	parent[0] = -1; 

	for (int count = 0; count < V1 - 1; count++) {

		int u = minKey(distanceP, visitedP);

		visitedP[u] = true;

		for (int v = 0; v < V1; v++)

			if (graph[u][v] && visitedP[v] == false && graph[u][v] < distanceP[v])
				parent[v] = u, distanceP[v] = graph[u][v];
	}

}

//-----------------------------------------------------------------------------------


string CityName[] = {"WareHouse", "Islamabad", "Lahore", "Karachi", "Multan", "Peshawar", "Quetta", "Abbotabad", "Skardu", "Gawadar", "Liaquat Bagh"};

string Abbotabad[]={"Abbotabad", "Comsats Atd", "PMA", "PC Hotel", "Ayub Medical College", "Fawara Chowk"};

string Islamabad[]={"Islamabad", "I-8" ,"Askari-14", "F-10", "F-7", "H-12"};

string Lahore[]={"Lahore", "Johar Town", "Garden Town", "DHA", "Model Town", "Comsats Lahore"};

string Karachi[]={"Karachi", "Bharia Twon", "North Nazimabad", "Defence", "Clifton", "Liyari"};

int graph[V][V]={   {0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 3},   //Warehouse
					{4, 0, 0, 0, 0, 0, 0, 6, 0, 0, 2},	 //Islamabad
					{0, 0, 0, 0, 9, 4, 0, 0, 0, 0, 11},	 //Lahore
					{0, 0, 0, 0, 1, 0, 0, 0, 16, 5, 0},	 //Karachi
					{0, 0, 9, 1, 0, 8, 13, 0, 10, 6, 0}, //Multan	
					{0, 0, 4, 0, 8, 0, 0, 3, 17, 0, 7},	 //Peshawar
					{0, 0, 0, 0, 13, 0, 0, 0, 0, 0, 7},	 //Quetta
					{0, 6, 0, 0, 0, 3, 0, 0, 5, 0, 0},	 //Abbotabad
					{0, 0, 0, 16, 10, 17, 0, 5, 0, 0, 0},//Skardu
					{0, 0, 0, 5, 6, 0, 0, 0, 0, 0, 0},	 //Gawadar
					{3, 2, 11, 0, 0, 7, 7, 0, 0, 0, 0}  };//Liaquat Bhag
					
//----------------------Abbotabad--------------------------------				
int AtdGraph[V1][V1]={ 	{ 0, 4, 4, 0, 0, 0 },
						{ 4, 0, 2, 0, 0, 0 },   //comsats
						{ 4, 2, 0, 3, 2, 4 },	//PMA
						{ 0, 0, 3, 0, 0, 3 },	//PC
						{ 0, 0, 2, 0, 0, 3 },	//Ayub
						{ 0, 0, 4, 3, 3, 0 } };	//Fawara
						
//---------------------Islamabad------------------------------------
int IsbGraph[V1][V1]={ 	{ 0, 4, 0, 2, 0, 0 },
						{ 4, 0, 3, 1, 0, 6 },	//I-8
						{ 0, 3, 0, 4, 0, 7 },	//Askari
						{ 2, 1, 4, 0, 9, 0 },	//F-10
						{ 0, 0, 0, 9, 0, 5 },	//F-7
						{ 0, 6, 7, 0, 5, 0 } };	//H-12

//-----------------------Lahore---------------------------------------
int LhrGraph[V1][V1]={ 	{ 0, 9, 15, 6, 0, 0 },
						{ 9, 0, 7, 0, 0, 8 },
						{ 15, 7, 0, 8, 9, 5 },
						{ 6, 0, 8, 0, 11, 0 },
						{ 0, 0, 9, 11, 0, 0 },
						{ 0, 8, 5, 0, 0, 0 } };	//Comsats Lahore

//----------------------Karachi----------------------------------------
int KarGraph[V1][V1]={ 	{ 0, 6, 3, 0, 0, 1 },
						{ 6, 0, 0, 2, 0, 5 },	//Bharia Town
						{ 3, 0, 0, 0, 3, 5 },	//North Nazimabad
						{ 0, 2, 0, 0, 6, 4 },	//Defence
						{ 0, 0, 3, 6, 0, 6 },	//Clifton
						{ 1, 5, 5, 4, 6, 0 } };	//Liyari

//----------------------------------------------------------------------------------

int city(){
	int opt;
	cout << "\nThe delivery is available for following Areas : \n" << endl;
	
	cout<<"\n---------------------------------";
	cout<<"\n |CITY CODE |        City       |";
	cout<<"\n---------------------------------";
    cout<<"\n |     1    |    Islamabad      |"<<endl;
    cout<<"\n |     2    |    Lahore         |"<<endl;
    cout<<"\n |     3    |    Karachi        |"<<endl;
	cout<<"\n |     4    |    Multan         |"<<endl;
    cout<<"\n |     5    |    Peshawar       |"<<endl;
    cout<<"\n |     6    |    Quetta         |"<<endl;
    cout<<"\n |     7    |    Abbotabad      |"<<endl;
    cout<<"\n |     8    |    Skardu         |"<<endl;
	cout<<"\n |     9    |    Gawadar        |"<<endl;
    cout<<"\n |    10    |    Liaquat Bhag   |"<<endl;
    cout<<"---------------------------------\n"<<endl;

    

	cout << "Enter your option :" << endl;
    cin >> opt;
    return opt;
}




int isb(){
	int opt;
	cout << "\nThe delivery is available for following Areas in Islamabad: " << endl;
    
    cout<<"\n---------------------------------";
	cout<<"\n |CITY CODE |        AREA       |";
	cout<<"\n---------------------------------";
    cout<<"\n |     1    |    I-8            |"<<endl;
    cout<<"\n |     2    |    Askari-14      |"<<endl;
    cout<<"\n |     3    |    F-10           |"<<endl;
	cout<<"\n |     4    |    F-7            |"<<endl;
    cout<<"\n |     5    |    H-12           |"<<endl;
    cout<<"---------------------------------\n"<<endl;
    
    
    cout << "Enter your option :" << endl;
    cin >> opt;
    return opt;
                              
}

int Atd(){
	int opt;
	cout << "\nThe delivery is available for following Areas in Abbotabad: " << endl;
	
	cout<<"\n---------------------------------";
	cout<<"\n |CITY CODE |        AREA       |";
	cout<<"\n---------------------------------";
    cout<<"\n |     1    |    Comsats Atd    |"<<endl;
    cout<<"\n |     2    |    PMA            |"<<endl;
    cout<<"\n |     3    |    PC-Hotel       |"<<endl;
	cout<<"\n |     4    |    Ayub Medical   |"<<endl;
    cout<<"\n |     5    |    Fawara Chowk   |"<<endl;
    cout<<"---------------------------------\n"<<endl;
    
    cout << "Enter your option :" << endl;
    cin >> opt;
    return opt;
                              
}

int Lhr(){
	int opt;
	cout << "\nThe delivery is available for following Areas in Lahore: " << endl;
	
	cout<<"\n---------------------------------";
	cout<<"\n |CITY CODE |        AREA       |";
	cout<<"\n---------------------------------";
    cout<<"\n |     1    |    Johar Town     |"<<endl;
    cout<<"\n |     2    |    Garden Town    |"<<endl;
    cout<<"\n |     3    |    DHA            |"<<endl;
	cout<<"\n |     4    |    Model Town     |"<<endl;
    cout<<"\n |     5    |    Comsats LHR    |"<<endl;
    cout<<"---------------------------------\n"<<endl;
	
    
    cout << "Enter your option :" << endl;
    cin >> opt;
    return opt;
                              
}

int Kar(){
	int opt;
	cout << "\nThe delivery is available for following Areas in Karachi: " << endl;
	
	cout<<"\n---------------------------------";
	cout<<"\n |CITY CODE |        AREA       |";
	cout<<"\n---------------------------------";
    cout<<"\n |     1    |    Bharia Town    |"<<endl;
    cout<<"\n |     2    |    North Nazimabad|"<<endl;
    cout<<"\n |     3    |    Defence        |"<<endl;
	cout<<"\n |     4    |    Clifton        |"<<endl;
    cout<<"\n |     5    |    Liyari         |"<<endl;
    cout<<"---------------------------------\n"<<endl;
    
    cout << "Enter your option :" << endl;
    cin >> opt;
    return opt;
                              
}


int FurthurArea(int option){
	int opt;
	switch(option){
			
			case 1: 
			{
				prims(IsbGraph,distanceP);
				opt=isb();
				addressF=Islamabad[opt];
			}
			break;
			
			case 2:
			{
				prims(LhrGraph,distanceP);
				opt=Lhr();
				addressF=Lahore[opt];
			}
			break;
			
			case 3:
			{
				prims(KarGraph,distanceP);
				opt=Kar();
				addressF=Karachi[opt];
			}
			break;
			
			case 7:
			{
				prims(AtdGraph,distanceP);
				opt=Atd();
				addressF=Abbotabad[opt];
			}
			break;
		}
		
	return opt;
}

// Function prototypes (declarations)
LRESULT CALLBACK LoginDlgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK RegisterDlgProc(HWND, UINT, WPARAM, LPARAM);
void ShowLoginDialog(HINSTANCE hInstance);
void ShowRegisterDialog(HINSTANCE hInstance);
void AdminMenu();
void UserMenu();
void MainMenu();
void AdminFunctions();
void UserFunctions();
void ExitApplication();
void CloseConsoleAndShowLogin();

// Define control IDs directly in the code

#define IDOK 1
#define IDCANCEL 2
#define IDEXIT 3


// Main function to initialize the window
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    hInst = hInstance;
    // Initialize the shop data
    shop = new Shop;
    shop->name = "WareHouse";
    shop->address = "Air Uni Islamabad, Islamabad";

    MainMenu();

    delete shop;
    return 0;
}

// Main menu to show the login dialog
void MainMenu() {
    ShowLoginDialog(hInst);
}
void registeruser() {
	ShowRegisterDialog(hInst);
}

// Function to show the login dialog
void ShowLoginDialog(HINSTANCE hInstance) {
	connectToDatabase();
    // Register the window class for the dialog
    WNDCLASS wc = {0};
    wc.lpfnWndProc = LoginDlgProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "LoginDialog";
    RegisterClass(&wc);

    // Create the dialog window
    HWND hDlg = CreateWindowEx(0, "LoginDialog", "Login", WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, 
                               CW_USEDEFAULT, CW_USEDEFAULT, 350, 200, nullptr, nullptr, hInstance, nullptr);

    if (hDlg) {
        ShowWindow(hDlg, SW_SHOW);
        UpdateWindow(hDlg);

        MSG msg;
        while (GetMessage(&msg, nullptr, 0, 0)) {
            if (!IsDialogMessage(hDlg, &msg)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    } 
	mysql_close(conn); 
}
void ShowRegisterDialog(HINSTANCE hInstance) {
	connectToDatabase();
    // Register the window class for the dialog
    WNDCLASS wc = {0};
    wc.lpfnWndProc = RegisterDlgProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "RegisterDialog";
    RegisterClass(&wc);

    // Create the dialog window
    HWND hDlg = CreateWindowEx(0, "RegisterDialog", "Register", WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, 
                               CW_USEDEFAULT, CW_USEDEFAULT, 350, 200, nullptr, nullptr, hInstance, nullptr);

    if (hDlg) {
        ShowWindow(hDlg, SW_SHOW);
        UpdateWindow(hDlg);

        MSG msg;
        while (GetMessage(&msg, nullptr, 0, 0)) {
            if (!IsDialogMessage(hDlg, &msg)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    } 
	mysql_close(conn); 
}
// Registration dialog procedure
LRESULT CALLBACK RegisterDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        CreateWindow("STATIC", "Username:", WS_VISIBLE | WS_CHILD, 10, 10, 80, 25, hDlg, NULL, hInst, NULL);
        CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 100, 10, 200, 25, hDlg, (HMENU)2001, hInst, NULL);
        CreateWindow("STATIC", "Password:", WS_VISIBLE | WS_CHILD, 10, 50, 80, 25, hDlg, NULL, hInst, NULL);
        CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_PASSWORD, 100, 50, 200, 25, hDlg, (HMENU)2002, hInst, NULL);
        CreateWindow("STATIC", "Email:", WS_VISIBLE | WS_CHILD, 10, 90, 80, 25, hDlg, NULL, hInst, NULL);
        CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 100, 90, 200, 25, hDlg, (HMENU)2003, hInst, NULL);
        CreateWindow("BUTTON", "Register", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 100, 130, 80, 25, hDlg, (HMENU)1, hInst, NULL);
        CreateWindow("BUTTON", "Cancel", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 220, 130, 80, 25, hDlg, (HMENU)2, hInst, NULL);
        return 0;

    case WM_COMMAND:
        if (LOWORD(wParam) == 1) { // Register button
            char username[100], password[100], email[100];
            GetDlgItemText(hDlg, 2001, username, 100);
            GetDlgItemText(hDlg, 2002, password, 100);
            GetDlgItemText(hDlg, 2003, email, 100);

            string query = "INSERT INTO Users (username, password, email) VALUES ('" +
                           string(username) + "', '" + string(password) + "', '" + string(email) + "')";
            executeQuery(query);
            MessageBox(hDlg, "User registered successfully!", "Success", MB_OK);
            DestroyWindow(hDlg);
            MainMenu();
        } else if (LOWORD(wParam) == 2) { // Cancel button
            DestroyWindow(hDlg);
        }
        break;

    case WM_CLOSE:
        DestroyWindow(hDlg);
        break;
    }

    return DefWindowProc(hDlg, message, wParam, lParam);
}
// Dialog procedure for handling login events
LRESULT CALLBACK LoginDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        CreateWindow("STATIC", "Username:", WS_VISIBLE | WS_CHILD, 10, 10, 80, 25, hDlg, nullptr, hInst, nullptr);
        CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 100, 10, 200, 25, hDlg, (HMENU)1001, hInst, nullptr);
        CreateWindow("STATIC", "Password:", WS_VISIBLE | WS_CHILD, 10, 50, 80, 25, hDlg, nullptr, hInst, nullptr);
        CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_PASSWORD, 100, 50, 200, 25, hDlg, (HMENU)1002, hInst, nullptr);
        CreateWindow("BUTTON", "OK", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 100, 90, 80, 25, hDlg, (HMENU)1, hInst, nullptr);
        CreateWindow("BUTTON", "Cancel", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 220, 90, 80, 25, hDlg, (HMENU)2, hInst, nullptr);
        CreateWindow("BUTTON", "Register", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 130, 300, 25, hDlg, (HMENU)3, hInst, nullptr);
        return 0;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
			    char username[100], password[100];
			    GetDlgItemText(hDlg, 1001, username, 100);
			    GetDlgItemText(hDlg, 1002, password, 100);
			
			    // Query to check username, password, and role
			    string query = "SELECT username, role FROM Users WHERE username = '" + string(username) +
			                   "' AND password = '" + string(password) + "'";
			    if (mysql_query(conn, query.c_str())) {
			        MessageBox(hDlg, "Database query failed!", "Error", MB_OK | MB_ICONERROR);
			        return 0;
			    }
			
			    MYSQL_RES* res = mysql_store_result(conn);
			    if (res && mysql_num_rows(res) > 0) {
			        MYSQL_ROW row = mysql_fetch_row(res);
			        string role = row[1]; // Fetch the role (e.g., 'admin' or 'customer')
			
			        if (role == "admin") {
			            MessageBox(hDlg, "Login successful! Welcome, Admin.", "Success", MB_OK);
			            DestroyWindow(hDlg); // Close the login window
			            AdminMenu(); // Launch Admin Menu on the console
			        } else if (role == "customer") {
			            MessageBox(hDlg, "Login successful! Welcome, Customer.", "Success", MB_OK);
			            DestroyWindow(hDlg); // Close the login window
			            UserMenu(); // Launch User Menu on the console
			        } else {
			            MessageBox(hDlg, "Invalid role!", "Error", MB_OK | MB_ICONERROR);
			        }
			    } else {
			        MessageBox(hDlg, "Invalid username or password!", "Error", MB_OK | MB_ICONERROR);
			    }
			    mysql_free_result(res);
			} else if (LOWORD(wParam) == 2) { // Cancel button
			    PostQuitMessage(0);
			} else if (LOWORD(wParam) == 3) { // Register button
			    // Open the registration dialog
			    DestroyWindow(hDlg);
			    registeruser();
			    
			}
        break;

    case WM_CLOSE:
        DestroyWindow(hDlg);
        return 0;
    }
    return DefWindowProc(hDlg, message, wParam, lParam);
}

// Admin menu function
void AdminMenu() {
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    cout << "Admin Menu Initialized" << endl; // Debug print
    AdminFunctions();
    FreeConsole();
    CloseConsoleAndShowLogin();
}

// User menu function
void UserMenu() {
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    cout << "User Menu Initialized" << endl; // Debug print
    UserFunctions();
    FreeConsole();
    CloseConsoleAndShowLogin();
}

// Function to close console and reopen login window
void CloseConsoleAndShowLogin() {
    FreeConsole();
    MainMenu();
}

// Exit application function
void ExitApplication() {
    PostQuitMessage(0);
}

void addProduct() {
    string newProductName;
    int newProductPrice;

    cout << "Enter the name of the new product: ";
    cin.ignore(); // Clear the input buffer
    getline(cin, newProductName);
    cout << "Enter the price of the new product: ";
    cin >> newProductPrice;
	
	shop->product.push_back(newProductName);
    shop->price.push_back(newProductPrice);
    
    // Add the new product and price to the vectors
	string query = "INSERT INTO Products (name, price) VALUES ('" +
                           string(newProductName) + "', '" + to_string(newProductPrice)+"')";
            executeQuery(query);
    cout << "Product added successfully!" << endl;
}

void updateProductPrice() {
    int productNumber;
    int newPrice;

    cout << "Enter the product number to update the price: ";
    cin >> productNumber;

    if (productNumber < 1 || productNumber >= shop->product.size()) {
        cout << "Invalid product number!" << endl;
        return;
    }

    cout << "Enter the new price for " << shop->product[productNumber] << ": ";
    cin >> newPrice;

    shop->price[productNumber] = newPrice;
    cout << "Price updated successfully!" << endl;
}

// Function to remove an existing product
void removeProduct() {
    int productNumber;

    cout << "Enter the product number to remove: ";
    cin >> productNumber;

    if (productNumber < 1 || productNumber >= shop->product.size()) {
        cout << "Invalid product number!" << endl;
        return;
    }

    shop->product[productNumber] = ""; // Remove product by setting it to an empty string
    shop->price[productNumber] = 0; // Reset price to 0
    cout << "Product removed successfully!" << endl;
}

void displayProducts() {
   cout << "\n---------------------------------------------------------------------";
   cout << "\n | ITEM NO. |             ITEM NAME               | ORIGINAL PRICE  |";
   cout << "\n---------------------------------------------------------------------";
for (size_t i = 1; i < shop->product.size(); ++i) {
    if (!shop->product[i].empty()) {
        cout << "\n | " << setw(8) << i << " | " << setw(30) << shop->product[i] << " | " << setw(15) << shop->price[i] << " |" << endl;
    }
}
cout << "---------------------------------------------------------------------" << endl;
}

void placeTakeAwayOrder(const string& userUsername) {
    int quantity;
    int productNumber;
    double bill;
    int orderId = nextOrderId++;  // Automatically generate the order ID

    cout << "Enter the product Number: ";
    cin >> productNumber;
    cout << "Enter the quantity of the product: ";
    cin >> quantity;

    bill = quantity * shop->price[productNumber];
    root = insertion(userUsername, quantity, shop->product[productNumber], bill, orderId, root);
    cout << "\033[1;35m"; // Set text color to purple
    cout << "\n------------------------------------" << endl;
    cout << "**** Your order has been placed *****" << endl;
    cout << "Total amount: " << bill << endl;
    cout << "Order ID: " << orderId << endl;
    cout << "You can get your order from Warehouse" << endl;
    cout << "------------------------------------\n" << endl;
    cout << "\033[0m"; // Reset text color
}
void placeHomeDeliveryOrder(const string& userUsername) {
    int quantity;
    int productNumber;
    double bill;
    string address;
    int distance;
    int orderId = nextOrderId++;  // Automatically generate the order ID

    dijkstra(graph, dist, 0);

    do {
        optionDelivery = city(); // Return the index of city that has been selected
    } while (!(optionDelivery >= 0 && optionDelivery <= 10));

    if (optionDelivery == 1 || optionDelivery == 2 || optionDelivery == 3 || optionDelivery == 7) {
        do {
            optionDelivery1 = FurthurArea(optionDelivery); // Return index of area in a city
        } while (!(optionDelivery1 >= 0 && optionDelivery1 <= 5));

        address = CityName[optionDelivery] + " " + addressF;
        distance = dist[optionDelivery] + distanceP[optionDelivery1];
    } else {
        address = CityName[optionDelivery];
        distance = dist[optionDelivery];
    }

    cout << "Enter the product Number: ";
    cin >> productNumber;
    cout << "Enter the quantity of the product: ";
    cin >> quantity;

    int deliveryChargesPerKM = 20;
    int deliveryCharges = deliveryChargesPerKM * distance;
    bill = quantity * shop->price[productNumber] + deliveryCharges;

    placeOrderHomeDeliveryCustomer(userUsername, shop->product[productNumber], quantity, bill, address, deliveryCharges, distance, userUsername);
    cout << "\033[1;35m"; // Set text color to purple
    cout << "\n------------------------------------" << endl;
    cout << "**** Your delivery order has been placed *****" << endl;
    cout << "Total amount: " << bill << endl;
    cout << "Order ID: " << orderId << endl;
    cout << "Delivery Address: " << address << endl;
    cout << "------------------------------------\n" << endl;
    cout << "\033[0m"; // Reset text color
}
void viewMaintenanceRequests() {
    if (!maintenanceHead) {
        cout << "No maintenance requests available." << endl;
        return;
    }

    MaintenanceRequest *current = maintenanceHead;
    cout << "\n---------------------------------------------------------------------";
    cout << "\n | REQUEST ID | USER NAME       | DESCRIPTION                      | REPLY  |";
    cout << "\n---------------------------------------------------------------------";
    while (current) {
        cout << "\n | " << current->requestId << "          | " << current->userName << "    | " << current->description << "   | " << current->reply << " |" << endl;
        current = current->next;
    }
    cout << "---------------------------------------------------------------------" << endl;
}


void sendMaintenanceRequest(const string& userUsername) {
    string description;

    cout << "Enter the description of the maintenance issue: ";
    cin.ignore(); // Clear the input buffer
    getline(cin, description);

    MaintenanceRequest *newRequest = new MaintenanceRequest(nextRequestId++, userUsername, description);
    newRequest->next = maintenanceHead;
    maintenanceHead = newRequest;

    cout << "Maintenance request sent successfully! Request ID: " << newRequest->requestId << endl;
}



// Function for admin to reply to maintenance requests
void replyToMaintenanceRequest() {
    int requestId;
    string reply;

    cout << "Enter the Request ID to reply: ";
    cin >> requestId;
    cin.ignore();  // Clear the input buffer

    MaintenanceRequest *current = maintenanceHead;
    while (current != nullptr) {
        if (current->requestId == requestId) {
            cout << "Enter your reply: ";
            getline(cin, reply);
            current->reply = reply;
            cout << "Reply sent successfully!" << endl;
            return;
        }
        current = current->next;
    }

    cout << "Request ID not found!" << endl;
}


// Function for users to view their maintenance requests and replies
void viewMyMaintenanceRequests(const string& userUsername) {
    if (!maintenanceHead) {
        cout << "No maintenance requests available." << endl;
        return;
    }

    MaintenanceRequest *current = maintenanceHead;
    cout << "\n---------------------------------------------------------------------";
    cout << "\n | REQUEST ID | DESCRIPTION                      | REPLY  |";
    cout << "\n---------------------------------------------------------------------";
    while (current) {
        if (current->userName == userUsername) {
            cout << "\n | " << current->requestId << "          | " << current->description << "   | " << current->reply << " |" << endl;
        }
        current = current->next;
    }
    cout << "---------------------------------------------------------------------" << endl;
}

// Admin functions (placeholder for actual functions)
void AdminFunctions() {
    int choice = -99;
    do { // Set text color to blue
        cout << "************************  Admin Menu  ************************" << endl;
        cout << "1. Add Product" << endl;
        cout << "2. Update Product Price" << endl;
        cout << "3. Remove Product" << endl;
        cout << "4. View All Products" << endl;
        cout << "5. View Maintenance Requests" << endl; // Option for viewing maintenance requests
        cout << "6. Reply to Maintenance Requests" << endl;
        cout << "7. Logout" << endl;
        cout << "8. Exit" << endl;
        cout << "Enter your choice: ";
 // Reset text color
        cin >> choice;

        switch (choice) {
            case 1:
                addProduct();
                break;
            case 2:
                updateProductPrice();
                break;
            case 3:
                removeProduct();
                break;
            case 4:
                displayProducts();
                break;
            case 5:
                viewMaintenanceRequests();
                break;
            case 6:
                replyToMaintenanceRequest();
                break;
            case 7:
                cout << "Logging out..." << endl;
                CloseConsoleAndShowLogin();
                return;
            case 8:
                cout << "Exiting application..." << endl;
                ExitApplication();
                break;
            default:
                cout << "Invalid choice, please try again." << endl;
        }
    } while (choice != 7 && choice != 8);
}

// User functions (placeholder for actual functions)
void UserFunctions() {
    int choice = -99;
    do { // Set text color to cyan
        cout << "************************  User Menu  ************************" << endl;
        cout << "1. Display Products and Prices" << endl;
        cout << "2. Place Take-Away Order" << endl;
        cout << "3. Place Home Delivery Order" << endl;
        cout << "4. Get Order from Warehouse" << endl;
        cout << "5. Display All Delivery Orders" << endl;
        cout << "6. Display All Take-Away Orders" << endl;
        cout << "7. Send Maintenance Request" << endl;
        cout << "8. View My Maintenance Requests" << endl;
        cout << "0. Logout" << endl;
        cout << "9. Exit" << endl;
        cout << "Enter your choice: ";// Reset text color
        cin >> choice;

        switch (choice) {
            case 1:
                displayProducts();
                break;
            case 2:
                placeTakeAwayOrder(loggedInUser);
                break;
            case 3:
                placeHomeDeliveryOrder(loggedInUser);
                break;
            case 4: {
                // Declare and initialize variables within the scope of the case
                int orderId;
                cout << "Enter the Order Id: ";
                cin >> orderId;

                takeAway* cust = search(root, orderId);

                if (cust == NULL) {
                    cout << "\n----------------------------------" << endl;
                    cout << "No such order is in the Warehouse" << endl;
                    cout << "-----------------------------------\n" << endl;
                } else {
                    root = deleteNode(root, orderId);
                    cout << "\n----------------------------------" << endl;
                    cout << "Your order is ready for collection" << endl;
                    cout << "Please get it from the counter" << endl;
                    cout << "-----------------------------------\n" << endl;
                }
                break;
            }
            case 5:
                displayAllOrdersHomeDeliveryCustomers();
                break;
            case 6:
                displayTakeAway(root);
                break;
            case 7:
                sendMaintenanceRequest(loggedInUser);
                break;
            case 8:
                viewMyMaintenanceRequests(loggedInUser);
                break;
            case 0:
                cout << "Logging out..." << endl;
                CloseConsoleAndShowLogin();
                return;
            case 9:
                cout << "Exiting application..." << endl;
                ExitApplication();
                break;
            default:
                cout << "Invalid choice, please try again." << endl;
        }
    } while (choice != 0 && choice != 9);
}
