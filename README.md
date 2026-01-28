# Appliance Shop Management System with Chat Feature (DSA Project)

## Project Overview
This C++ project simulates an **Appliance Shop Management System** with full user management, product management, and order handling. It supports both **home delivery** and **takeaway orders** and includes a **real-time chat feature** for admin-customer communication. The system uses **advanced data structures and algorithms** to optimize operations.

---

## Main Features

### 1. User Authentication
- Login and registration system with **MySQL database** storage.
- Role-based access: **Admin** and **Customer**.
- Secure password handling.

### 2. Admin Functionalities
- Add, update, and remove products.
- View all orders (takeaway & home delivery).
- Manage delivery assignments.
- **Real-time chat** with customers.

### 3. Customer Functionalities
- Place **takeaway orders** using an AVL tree.
- Place **home delivery orders** with distance-based delivery charges.
- Select **city and area** for delivery.
- View order details.
- **Real-time chat** with admin for queries or complaints.

### 4. Order Management
- **Takeaway orders** stored in **AVL tree** → fast search, insertion, deletion.
- **Home delivery orders** stored in **linked list** → simple traversal and update.
- Automatic **Order ID generation** for tracking.

### 5. Delivery Management
- **Dijkstra's algorithm** → calculates shortest path to cities.
- **Prim's Minimum Spanning Tree (MST)** → optimizes local area routing.
- Supports multiple cities: Islamabad, Lahore, Karachi, Multan, Peshawar, Quetta, Abbotabad, Skardu, Gawadar, Liaquat Bagh.

### 6. Chat Feature
- **Admin-Customer chat** implemented using console-based input/output.
- Admin can respond to customer queries about orders or products.
- Chat logs can be extended to database storage for persistence.

### 7. Data Structures Used
- **AVL Tree** → Takeaway orders.
- **Linked List** → Home delivery orders.
- **Graph** → City and area routing for delivery.
- **Vectors & Arrays** → Product lists, distances, and city names.
- **String & Structs** → Store user, product, and order details.

### 8. Algorithms Implemented
- **AVL Tree rotations** → maintain balanced takeaway orders.
- **Dijkstra’s algorithm** → compute shortest delivery paths.
- **Prim’s algorithm** → optimize local area routing.
- **Console-based menus** → smooth navigation for admin and customer.

---

## Setup Instructions
1. Install **MySQL** and create a database `ApplianceShop` with tables:
   - `Users(username, password, email, role)`
   - `Products(name, price)`
2. Add MySQL Connector/C++ to your compiler include path.
3. Compile the project using any C++ compiler supporting **Windows API** and **MySQL integration**.
4. Run the executable and use the **Login/Register menus** to access admin or customer functions.

---

## How to Use
1. **Login/Register** as Admin or Customer.
2. **Admin** can manage products, view orders, and chat with customers.
3. **Customer** can place takeaway or home delivery orders and communicate with admin via chat.
4. **Order management** and **delivery routing** are automated using AVL tree, linked list, Dijkstra, and Prim algorithms.

---

## Technologies & Libraries
- **C++** (Standard & Windows API for dialogs)
- **MySQL** (for database integration)
- **DSA concepts**: AVL Tree, Linked List, Graph Algorithms (Dijkstra & Prim)
- **Console I/O** for chat and order interaction
