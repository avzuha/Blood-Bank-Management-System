import mysql.connector as sql

# Connect to MySQL database
mycon = sql.connect(
    host="localhost",
    user="root",
    passwd="123456",
    database="BLOODBANK"
)

if not mycon.is_connected():
    print("Error in connection.")

cursor = mycon.cursor()

# Create tables if they don't exist
cursor.execute('''
    CREATE TABLE IF NOT EXISTS acceptor (
        Sno INT(5),
        Name VARCHAR(25),
        Blood_Type VARCHAR(3),
        Units_taken INT(3),
        Hospital_Name VARCHAR(50),
        Contacts INT(10),
        Retrieval_Date VARCHAR(15)
    )
''')

cursor.execute('''
    CREATE TABLE IF NOT EXISTS donor (
        donor_id INT PRIMARY KEY,
        name VARCHAR(50),
        Blood_type VARCHAR(3),
        Age INT,
        Gender CHAR(1),
        DOB VARCHAR(10),
        Contacts INT(10)
    )
''')

cursor.execute('''
    CREATE TABLE IF NOT EXISTS blood (
        Blood_type VARCHAR(3) PRIMARY KEY,
        Quantity INT(2)
    )
''')

cursor.execute('''
    CREATE TABLE IF NOT EXISTS donation (
        Donor_id INT(5),
        Blood_type VARCHAR(3),
        Units_Donated INT(2),
        Last_Donation_date VARCHAR(10)
    )
''')

mycon.commit()


# ---------------- Functions ---------------- #

def newDonor():
    """Register a new donor."""
    n = input("Enter your name: ")
    d = input("Enter DOB (YYYY-MM-DD): ")

    queryy = f"SELECT DATEDIFF(CURDATE(), '{d}')"
    cursor.execute(queryy)
    age = cursor.fetchone()
    a = age[0] // 365

    if 18 < a < 65:
        ta = input("Do you have a tattoo? (yes/no): ")
        if ta.lower() == "no":
            cursor.execute("SELECT donor_id FROM donor ORDER BY donor_id DESC LIMIT 1")
            result = cursor.fetchone()
            idd = 1 if result is None else result[0] + 1

            g = input("Enter gender (F/M): ")
            b = input("Enter blood type: ")
            c = int(input("Enter contact: "))

            query = f"""
                INSERT INTO donor VALUES (
                    {idd}, '{n}', '{b}', {a}, '{g}', '{d}', {c}
                )
            """
            cursor.execute(query)
            mycon.commit()

            print("Your Donor_id is:", idd)

            askk = input("Do you want to donate now (y/n)? ")
            if askk.lower() == 'y':
                existingdonor()
            elif askk.lower() == 'n':
                pass
            else:
                print("Enter correct choice.")
        else:
            print("You cannot donate blood.")
    else:
        print("You need to be between 18 and 65 to donate blood.")


def existingdonor():
    """Handle donation from existing donors."""
    d = input("Have you donated before? (yes/no): ")

    if d.lower() == 'yes':
        do = int(input("Enter donor id: "))
        query = "SELECT * FROM donation WHERE Donor_id = %s"
        cursor.execute(query, (do,))
        result = cursor.fetchone()

        if result:
            last_date = result[3]

            cursor.execute("SELECT CURDATE()")
            curd = str(cursor.fetchone()[0])

            dif = f"SELECT DATEDIFF('{curd}', '{last_date}')"
            cursor.execute(dif)
            dd = cursor.fetchone()

            if dd[0] > 60:
                units = int(input("Enter units of blood donated: "))
                cursor.execute("SELECT CURDATE()")
                curd = str(cursor.fetchone()[0])

                # Example insert (fix variables as needed)
                query = f"""
                    INSERT INTO donation VALUES (
                        {do}, '{result[1]}', {units}, '{curd}'
                    )
                """
                cursor.execute(query)
                mycon.commit()

                print("Your donation is successful.")
            else:
                print("You cannot donate blood for now.")
    else:
        did = int(input("Enter your donor_id: "))
        query = "SELECT donor_id FROM donor WHERE donor_id = %s"
        cursor.execute(query, (did,))
        result = cursor.fetchone()

        if result:
            query = f"SELECT blood_type FROM donor WHERE donor_id = {did}"
            cursor.execute(query)
            data = cursor.fetchone()

            units = int(input("Enter units of blood donated: "))
            cursor.execute("SELECT CURDATE()")
            curd = str(cursor.fetchone()[0])

            query = f"""
                INSERT INTO donation VALUES (
                    {did}, '{data[0]}', {units}, '{curd}'
                )
            """
            cursor.execute(query)
            mycon.commit()

            print("Your donation is successful.")
        else:
            print(f"Donor with id '{did}' not found in the database.")


def receiver():
    """Handle blood requests from receivers."""
    cursor.execute("SELECT * FROM blood")
    data = cursor.fetchall()

    blood_type = input("Enter your blood type: ")
    quantity_requested = int(input("Enter the quantity of blood required in units (1 unit = 450 mL): "))
    name = input("Enter your name: ")
    hospital = input("Enter the hospital name: ")
    contacts = int(input("Enter your contact: "))

    cursor.execute("SELECT CURDATE()")
    da = cursor.fetchone()[0]

    cursor.execute(f"SELECT quantity FROM blood WHERE Blood_type = '{blood_type}'")
    result = cursor.fetchone()

    if result:
        current_quantity = result[0]
        if current_quantity >= quantity_requested:
            new_quantity = current_quantity - quantity_requested

            cursor.execute("SELECT donor_id FROM donor ORDER BY donor_id DESC LIMIT 1")
            result = cursor.fetchone()
            idd = 1 if result is None else result[0] + 1

            cursor.execute(f"""
                UPDATE blood SET quantity = {new_quantity}
                WHERE Blood_type = '{blood_type}'
            """)
            mycon.commit()

            query = f"""
                INSERT INTO acceptor VALUES (
                    {idd}, '{name}', '{blood_type}', {quantity_requested},
                    '{hospital}', {contacts}, '{da}'
                )
            """
            cursor.execute(query)
            mycon.commit()

            print("Your receiver id is:", idd)
            print(f"{quantity_requested} units deducted from {blood_type}. Stock updated successfully.")
        else:
            print(f"Insufficient stock. Only {current_quantity} units available for {blood_type}.")
    else:
        print(f"No record found for blood type {blood_type}.")


def update_d():
    """Update donor details."""
    did = int(input("Enter donor_id: "))
    a = input("Enter what do you wish to change (Name/Age/Contact): ")
    
    if a.lower() == 'name':
        new = input("Enter new name: ")
        query = f"UPDATE donor SET name='{new}' WHERE donor_id={did}"
        cursor.execute(query)
        mycon.commit()
        print("Updated successfully.")

    elif a.lower() == 'age':
        new = int(input("Enter new age: "))
        query = f"UPDATE donor SET age={new} WHERE donor_id={did}"
        cursor.execute(query)
        mycon.commit()
        print("Updated successfully.")

    elif a.lower() == 'contact':
        new = int(input("Enter new contact: "))
        query = f"UPDATE donor SET contact={new} WHERE donor_id={did}"
        cursor.execute(query)
        mycon.commit()
        print("Updated successfully.")

    else:
        print("Enter correct choice.")


def update_r():
    """Update receiver details."""
    did = int(input("Enter receiver_id: "))
    q = f"SELECT sno FROM acceptor WHERE sno={did}"
    cursor.execute(q)
    dd = cursor.fetchone()

    if dd is None:
        print("Incorrect receiver id entered.")
    else:
        a = input("Enter what do you wish to change (Name/Contact): ")

        if a.lower() == 'name':
            new = input("Enter new name: ")
            query = f"UPDATE acceptor SET name='{new}' WHERE sno={did}"
            cursor.execute(query)
            mycon.commit()
            print("Updated successfully.")

        elif a.lower() == 'contact':
            new = int(input("Enter new contact: "))
            query = f"UPDATE acceptor SET contacts={new} WHERE sno={did}"
            cursor.execute(query)
            mycon.commit()
            print("Updated successfully.")

        else:
            print("Enter correct choice.")


def deleteblood(blood_type):
    """Remove expired blood units."""
    b = int(input("How many units have expired? "))
    query = f"UPDATE blood SET quantity=quantity-{b} WHERE blood_type='{blood_type}'"
    cursor.execute(query)
    mycon.commit()


def search_blood_type(blood_type):
    """Search availability of a specific blood type."""
    query = "SELECT Quantity FROM blood WHERE Blood_type = %s"
    cursor.execute(query, (blood_type,))
    result = cursor.fetchone()

    if result:
        print(f"Available quantity of blood type {blood_type}: {result[0]} units.")
    else:
        print(f"Blood type {blood_type} not found in the database.")

    ask = input("Do you wish to remove any expired blood (Y/N)? ")
    if ask.lower() == 'y':
        deleteblood(blood_type)

    # Show updated stock
    cursor.execute(query, (blood_type,))
    result = cursor.fetchone()
    if result:
        print(f"Available quantity of blood type {blood_type}: {result[0]} units.")
    else:
        print(f"Blood type {blood_type} not found in the database.")


def search_donor_by_id(Did):
    """Search donor details by ID."""
    query = "SELECT * FROM donor WHERE donor_id = %s"
    cursor.execute(query, (Did,))
    result = cursor.fetchone()

    if result:
        print(f"Details of donor '{Did}':")
        print(f"ID: {result[0]}\nName: {result[1]}\nBlood Type: {result[2]}\nAge: {result[3]}\n"
              f"Gender: {result[4]}\nDOB: {result[5]}\nContact: {result[6]}")
    else:
        print(f"Donor with id '{Did}' not found in the database.")


def search_for_units():
    """Search donation details by donor ID."""
    Did = int(input("Enter donor id: "))
    query = "SELECT * FROM donation WHERE donor_id = %s"
    cursor.execute(query, (Did,))
    result = cursor.fetchone()

    if result:
        print(f"Details of donation '{Did}':")
        print(f"Donor id: {result[0]}\nBlood Type: {result[1]}\nUnits Donated: {result[2]}\nLast Donation Date: {result[3]}")
    else:
        print(f"Donor with id '{Did}' not found in the database.")


# ---------------- Main Menu ---------------- #

print("\t\t *** BLOOD BANK MANAGEMENT ***")

t = "y"
while t.lower() == "y":
    print("\n\t\t\t 1. For New Donor details.")
    print("\t\t\t 2. Add Existing donor details.")
    print("\t\t\t 3. For Receiver details.")
    print("\t\t\t 4. Update donor details.")
    print("\t\t\t 5. Update receiver details.")
    print("\t\t\t 6. Search Blood availability.")
    print("\t\t\t 7. Search Amount of Blood donated.")
    print("\t\t\t 8. Search Donor by ID.")
    print("\t\t\t 9. Exit")

    s = int(input("Enter what you wish to access: "))

    if s == 1:
        newDonor()
    elif s == 2:
        existingdonor()
    elif s == 3:
        receiver()
    elif s == 4:
        update_d()
    elif s == 5:
        update_r()
    elif s == 6:
        blood = input("Enter blood type: ")
        search_blood_type(blood)
    elif s == 7:
        search_for_units()
    elif s == 8:
        Did = int(input("Enter id of donor: "))
        search_donor_by_id(Did)
    elif s == 9:
        cursor.close()
        mycon.close()
        print("Exiting Blood Bank Management System.")
        break
    else:
        print("Enter correct option.")
