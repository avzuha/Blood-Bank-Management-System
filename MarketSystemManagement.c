#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LEN 100
#define LINE_LEN 512

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void read_line(char *buffer, int size) {
    if (fgets(buffer, size, stdin)) {
        size_t ln = strlen(buffer);
        if (ln > 0 && buffer[ln - 1] == '\n') buffer[ln - 1] = '\0';
    }
}

int next_id_from_file(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return 1;
    int maxid = 0;
    char line[LINE_LEN];
    while (fgets(line, sizeof(line), fp)) {
        int id = 0;
        sscanf(line, "%d|", &id);
        if (id > maxid) maxid = id;
    }
    fclose(fp);
    return maxid + 1;
}

/*---------------- Supplier Module ----------------*/
void add_supplier_product() {
    FILE *fp = fopen("supplier.txt", "a");
    if (!fp) { printf("Error: could not open supplier.txt\n"); return; }

    int id = next_id_from_file("supplier.txt");
    char name[NAME_LEN];
    int qty;
    float cost;

    printf("Enter product name: ");
    read_line(name, NAME_LEN);

    printf("Enter quantity: ");
    if (scanf("%d", &qty) != 1) {
        clear_input_buffer();
        printf("Invalid quantity.\n");
        fclose(fp);
        return;
    }

    printf("Enter cost price: ");
    if (scanf("%f", &cost) != 1) {
        clear_input_buffer();
        printf("Invalid cost.\n");
        fclose(fp);
        return;
    }

    clear_input_buffer();
    fprintf(fp, "%d|%s|%d|%.2f\n", id, name, qty, cost);
    fclose(fp);

    printf("Supplier product added with ID %d\n", id);
}

void view_suppliers() {
    FILE *fp = fopen("supplier.txt", "r");
    if (!fp) { printf("No supplier records found.\n"); return; }

    char line[LINE_LEN];
    printf("\n-- Supplier Stock --\n");
    printf("ID\tName\t\tQty\tCost/unit\n");

    while (fgets(line, sizeof(line), fp)) {
        int id, qty;
        char name[NAME_LEN];
        float cost;
        if (sscanf(line, "%d|%99[^|]|%d|%f", &id, name, &qty, &cost) == 4) {
            printf("%d\t%-15s\t%d\t%.2f\n", id, name, qty, cost);
        }
    }
    fclose(fp);
}

/*---------------- Transport Module ----------------*/
void add_transport() {
    FILE *fp = fopen("transport.txt", "a");
    if (!fp) { printf("Error: could not open transport.txt\n"); return; }

    int id = next_id_from_file("transport.txt");
    char vehicle[NAME_LEN], driver[NAME_LEN], source[NAME_LEN], dest[NAME_LEN], status[30];
    float cost;

    printf("Enter vehicle number: ");
    read_line(vehicle, NAME_LEN);
    printf("Enter driver name: ");
    read_line(driver, NAME_LEN);
    printf("Enter source: ");
    read_line(source, NAME_LEN);
    printf("Enter destination: ");
    read_line(dest, NAME_LEN);

    printf("Enter transport cost: ");
    if (scanf("%f", &cost) != 1) {
        clear_input_buffer();
        printf("Invalid cost.\n");
        fclose(fp);
        return;
    }

    clear_input_buffer();
    strcpy(status, "Pending");

    fprintf(fp, "%d|%s|%s|%s|%s|%.2f|%s\n", id, vehicle, driver, source, dest, cost, status);
    fclose(fp);

    printf("Transport record added with ID %d (Status: Pending)\n", id);
}

void view_transports() {
    FILE *fp = fopen("transport.txt", "r");
    if (!fp) { printf("No transport records found.\n"); return; }

    char line[LINE_LEN];

    printf("\n-- Transport Records --\n");
    printf("ID | Vehicle | Driver | Source->Dest | Cost | Status\n");

    while (fgets(line, sizeof(line), fp)) {
        int id;
        char vehicle[NAME_LEN], driver[NAME_LEN], source[NAME_LEN], dest[NAME_LEN], status[30];
        float cost;

        if (sscanf(line, "%d|%99[^|]|%99[^|]|%99[^|]|%99[^|]|%f|%29[^\n]",
                   &id, vehicle, driver, source, dest, &cost, status) == 7) {
            printf("%d | %s | %s | %s->%s | %.2f | %s\n",
                   id, vehicle, driver, source, dest, cost, status);
        }
    }
    fclose(fp);
}

void update_transport_status() {
    int tid;
    printf("Enter Transport ID to update: ");

    if (scanf("%d", &tid) != 1) {
        clear_input_buffer();
        printf("Invalid ID.\n");
        return;
    }
    clear_input_buffer();

    FILE *fp = fopen("transport.txt", "r");
    FILE *tmp = fopen("temp_transport.txt", "w");

    if (!fp || !tmp) {
        printf("Error accessing transport records.\n");
        if (fp) fclose(fp);
        if (tmp) fclose(tmp);
        return;
    }

    char line[LINE_LEN];
    int found = 0;

    while (fgets(line, sizeof(line), fp)) {
        int id;
        char vehicle[NAME_LEN], driver[NAME_LEN], source[NAME_LEN], dest[NAME_LEN], status[30];
        float cost;

        if (sscanf(line, "%d|%99[^|]|%99[^|]|%99[^|]|%99[^|]|%f|%29[^\n]",
                   &id, vehicle, driver, source, dest, &cost, status) == 7) {
            if (id == tid) {
                found = 1;
                char newstatus[30];
                printf("Current status: %s\n", status);
                printf("New status: ");
                read_line(newstatus, 30);

                fprintf(tmp, "%d|%s|%s|%s|%s|%.2f|%s\n",
                        id, vehicle, driver, source, dest, cost, newstatus);
            } else {
                fprintf(tmp, "%s", line);
            }
        }
    }

    fclose(fp);
    fclose(tmp);

    remove("transport.txt");
    rename("temp_transport.txt", "transport.txt");

    if (!found)
        printf("Transport ID %d not found.\n", tid);
}

/*---------------- Wholesaler Module ----------------*/
void wholesaler_receive_from_supplier() {
    FILE *fs = fopen("supplier.txt", "r");
    if (!fs) { printf("No supplier products available.\n"); return; }

    char line[LINE_LEN];

    printf("-- Supplier Products --\n");
    while (fgets(line, sizeof(line), fs)) {
        int id, qty;
        char name[NAME_LEN];
        float cost;
        if (sscanf(line, "%d|%99[^|]|%d|%f", &id, name, &qty, &cost) == 4) {
            printf("ID:%d | %s | Qty:%d | Cost:%.2f\n", id, name, qty, cost);
        }
    }
    fclose(fs);

    int sid;
    printf("Enter Supplier Product ID to receive: ");
    if (scanf("%d", &sid) != 1) {
        clear_input_buffer();
        printf("Invalid ID.\n");
        return;
    }
    clear_input_buffer();

    FILE *fp = fopen("supplier.txt", "r");
    FILE *tmp = fopen("temp_supplier.txt", "w");
    if (!fp || !tmp) {
        printf("Error accessing supplier records.\n");
        if (fp) fclose(fp);
        if (tmp) fclose(tmp);
        return;
    }

    int found = 0;
    int rec_qty = 0;
    char rec_name[NAME_LEN];
    float rec_cost = 0.0f;

    while (fgets(line, sizeof(line), fp)) {
        int id, qty;
        char name[NAME_LEN];
        float cost;

        if (sscanf(line, "%d|%99[^|]|%d|%f", &id, name, &qty, &cost) == 4) {
            if (id == sid) {
                found = 1;
                strcpy(rec_name, name);
                rec_qty = qty;
                rec_cost = cost;
                /* don't write - removing entry */
            } else {
                fprintf(tmp, "%s", line);
            }
        }
    }

    fclose(fp);
    fclose(tmp);

    if (!found) {
        remove("temp_supplier.txt");
        printf("Supplier product ID not found.\n");
        return;
    }

    remove("supplier.txt");
    rename("temp_supplier.txt", "supplier.txt");

    float cutting_cost, sell_price;

    printf("Received product: %s (Qty:%d) Cost/unit: %.2f\n",
           rec_name, rec_qty, rec_cost);

    printf("Enter cutting/processing cost per unit: ");
    if (scanf("%f", &cutting_cost) != 1) {
        clear_input_buffer();
        printf("Invalid cost.\n");
        return;
    }

    printf("Enter wholesaler selling price per unit: ");
    if (scanf("%f", &sell_price) != 1) {
        clear_input_buffer();
        printf("Invalid price.\n");
        return;
    }
    clear_input_buffer();

    FILE *fw = fopen("wholesale.txt", "a");
    if (!fw) { printf("Error writing wholesale.txt\n"); return; }

    int wid = next_id_from_file("wholesale.txt");
    float new_cost = rec_cost + cutting_cost;

    fprintf(fw, "%d|%s|%d|%.2f|%.2f\n",
            wid, rec_name, rec_qty, new_cost, sell_price);

    fclose(fw);
    printf("Added to wholesaler stock with ID %d\n", wid);
}

void view_wholesale_stock() {
    FILE *fp = fopen("wholesale.txt", "r");
    if (!fp) { printf("No wholesale stock found.\n"); return; }

    char line[LINE_LEN];

    printf("\n-- Wholesaler Stock --\n");
    printf("ID\tName\tQty\tCost/unit\tSell/unit\n");

    while (fgets(line, sizeof(line), fp)) {
        int id, qty;
        char name[NAME_LEN];
        float cost, sell;

        if (sscanf(line, "%d|%99[^|]|%d|%f|%f", &id, name, &qty, &cost, &sell) == 5) {
            printf("%d\t%-12s\t%d\t%.2f\t\t%.2f\n",
                   id, name, qty, cost, sell);
        }
    }
    fclose(fp);
}

/*---------------- Retailer Module ----------------*/
void retailer_receive_from_wholesaler() {
    FILE *fw = fopen("wholesale.txt", "r");
    if (!fw) { printf("No wholesale products available.\n"); return; }

    char line[LINE_LEN];

    printf("-- Wholesale Products --\n");
    while (fgets(line, sizeof(line), fw)) {
        int id, qty;
        char name[NAME_LEN];
        float cost, sell;

        if (sscanf(line, "%d|%99[^|]|%d|%f|%f", &id, name, &qty, &cost, &sell) == 5) {
            printf("ID:%d | %s | Qty:%d | Cost:%.2f | Sell:%.2f\n",
                   id, name, qty, cost, sell);
        }
    }
    fclose(fw);

    int wid;
    printf("Enter Wholesale Product ID to receive: ");
    if (scanf("%d", &wid) != 1) {
        clear_input_buffer();
        printf("Invalid ID\n");
        return;
    }
    clear_input_buffer();

    FILE *fp = fopen("wholesale.txt", "r");
    FILE *tmp = fopen("temp_whole.txt", "w");

    if (!fp || !tmp) {
        printf("Error accessing wholesale records.\n");
        if (fp) fclose(fp);
        if (tmp) fclose(tmp);
        return;
    }

    int found = 0;
    int rec_qty = 0;
    char rec_name[NAME_LEN];
    float rec_cost = 0.0f, rec_sell = 0.0f;

    while (fgets(line, sizeof(line), fp)) {
        int id, qty;
        char name[NAME_LEN];
        float cost, sell;

        if (sscanf(line, "%d|%99[^|]|%d|%f|%f", &id, name, &qty, &cost, &sell) == 5) {
            if (id == wid) {
                found = 1;
                strcpy(rec_name, name);
                rec_qty = qty;
                rec_cost = cost;
                rec_sell = sell;
            } else {
                fprintf(tmp, "%s", line);
            }
        }
    }

    fclose(fp);
    fclose(tmp);

    if (!found) {
        remove("temp_whole.txt");
        printf("Wholesale product ID not found.\n");
        return;
    }

    remove("wholesale.txt");
    rename("temp_whole.txt", "wholesale.txt");

    FILE *fr = fopen("retail.txt", "a");
    if (!fr) { printf("Error: cannot open retail.txt\n"); return; }

    int rid = next_id_from_file("retail.txt");

    float retailer_sell;
    printf("Received product: %s (Qty:%d) Cost/unit:%.2f (Wholesale sell:%.2f)\n",
           rec_name, rec_qty, rec_cost, rec_sell);
    printf("Enter retailer selling price (0 to keep %.2f): ", rec_sell);

    if (scanf("%f", &retailer_sell) != 1) {
        clear_input_buffer();
        printf("Invalid price.\n");
        fclose(fr);
        return;
    }
    clear_input_buffer();

    if (retailer_sell <= 0.0f) retailer_sell = rec_sell;

    fprintf(fr, "%d|%s|%d|%.2f|%.2f\n",
            rid, rec_name, rec_qty, rec_cost, retailer_sell);

    fclose(fr);

    printf("Added to retailer stock with ID %d\n", rid);
}

void view_retail_stock() {
    FILE *fp = fopen("retail.txt", "r");
    if (!fp) { printf("No retail stock found.\n"); return; }

    char line[LINE_LEN];

    printf("\n-- Retail Stock --\n");
    printf("ID\tName\tQty\tCost/unit\tSell/unit\n");

    while (fgets(line, sizeof(line), fp)) {
        int id, qty;
        char name[NAME_LEN];
        float cost, sell;

        if (sscanf(line, "%d|%99[^|]|%d|%f|%f", &id, name, &qty, &cost, &sell) == 5) {
            printf("%d\t%-12s\t%d\t%.2f\t\t%.2f\n",
                   id, name, qty, cost, sell);
        }
    }
    fclose(fp);
}

/*---------------- Billing Module ----------------*/
void generate_bill_for_consumer() {
    view_retail_stock();

    int rid;
    printf("Enter Retail Stock ID to sell: ");
    if (scanf("%d", &rid) != 1) {
        clear_input_buffer();
        printf("Invalid ID.\n");
        return;
    }
    clear_input_buffer();

    FILE *fp = fopen("retail.txt", "r");
    FILE *tmp = fopen("temp_retail.txt", "w");

    if (!fp || !tmp) {
        printf("Error accessing retail records.\n");
        if (fp) fclose(fp);
        if (tmp) fclose(tmp);
        return;
    }

    char line[LINE_LEN];
    int found = 0, available_qty = 0;
    char item_name[NAME_LEN];
    float cost_price = 0, sell_price = 0;

    while (fgets(line, sizeof(line), fp)) {
        int id, qty;
        char name[NAME_LEN];
        float cost, sell;

        if (sscanf(line, "%d|%99[^|]|%d|%f|%f", &id, name, &qty, &cost, &sell) == 5) {
            if (id == rid) {
                found = 1;
                available_qty = qty;
                strcpy(item_name, name);
                cost_price = cost;
                sell_price = sell;
            } else {
                fprintf(tmp, "%s", line);
            }
        }
    }
    fclose(fp);

    if (!found) {
        fclose(tmp);
        remove("temp_retail.txt");
        printf("Retail ID not found.\n");
        return;
    }

    int sell_qty;
    printf("Available qty of %s: %d\n", item_name, available_qty);
    printf("Enter quantity to sell: ");

    if (scanf("%d", &sell_qty) != 1) {
        clear_input_buffer();
        printf("Invalid qty.\n");
        fclose(tmp);
        return;
    }
    clear_input_buffer();

    if (sell_qty <= 0 || sell_qty > available_qty) {
        printf("Invalid sale quantity.\n");
        fclose(tmp);
        remove("temp_retail.txt");
        return;
    }

    char buyer[NAME_LEN];
    printf("Enter buyer name: ");
    read_line(buyer, NAME_LEN);

    float total = sell_qty * sell_price;
    float profit = sell_qty * (sell_price - cost_price);

    int new_qty = available_qty - sell_qty;

    if (new_qty > 0) {
        fprintf(tmp, "%d|%s|%d|%.2f|%.2f\n",
                rid, item_name, new_qty, cost_price, sell_price);
    }

    fclose(tmp);

    remove("retail.txt");
    rename("temp_retail.txt", "retail.txt");

    FILE *fb = fopen("bills.txt", "a");
    if (!fb) { printf("Error opening bills.txt\n"); return; }

    int bid = next_id_from_file("bills.txt");

    /* Format: BillID|SellerType|SellerName|Buyer|Item|Qty|Unit|Total|Profit */
    fprintf(fb, "%d|Retailer|LocalRetail|%s|%s|%d|%.2f|%.2f|%.2f\n",
            bid, buyer, item_name, sell_qty, sell_price, total, profit);

    fclose(fb);

    printf("\n--- BILL ---\n");
    printf("BillID: %d\nBuyer: %s\nItem: %s\nQty: %d\nUnit: %.2f\nTotal: %.2f\nProfit: %.2f\n",
           bid, buyer, item_name, sell_qty, sell_price, total, profit);
}

/*---------------- Report Module ----------------*/
void view_bills() {
    FILE *fp = fopen("bills.txt", "r");
    if (!fp) { printf("No bills found.\n"); return; }

    char line[LINE_LEN];

    printf("\n-- Bills --\n");
    printf("BillID|SellerType|SellerName|Buyer|Item|Qty|Unit|Total|Profit\n");

    while (fgets(line, sizeof(line), fp)) {
        int bid, qty;
        char sType[NAME_LEN], sName[NAME_LEN], buyer[NAME_LEN], item[NAME_LEN];
        float unit, total, profit;

        if (sscanf(line, "%d|%99[^|]|%99[^|]|%99[^|]|%99[^|]|%d|%f|%f|%f",
                   &bid, sType, sName, buyer, item, &qty, &unit, &total, &profit) == 9) {
            printf("%d|%s|%s|%s|%s|%d|%.2f|%.2f|%.2f\n",
                   bid, sType, sName, buyer, item, qty, unit, total, profit);
        }
    }
    fclose(fp);
}

void summary_report() {
    FILE *fb = fopen("bills.txt", "r");

    float totalSales = 0, totalProfit = 0;
    char line[LINE_LEN];

    if (fb) {
        while (fgets(line, sizeof(line), fb)) {
            int b, qty;
            char tmp1[NAME_LEN];
            float unit, total, profit;

            if (sscanf(line, "%d|%[^|]|%[^|]|%[^|]|%[^|]|%d|%f|%f|%f",
                       &b, tmp1, tmp1, tmp1, tmp1,
                       &qty, &unit, &total, &profit) == 9) {
                totalSales += total;
                totalProfit += profit;
            }
        }
        fclose(fb);
    }

    int lowCount = 0;
    FILE *fr = fopen("retail.txt", "r");
    if (fr) {
        while (fgets(line, sizeof(line), fr)) {
            int id, qty;
            char name[NAME_LEN];
            float c, s;

            if (sscanf(line, "%d|%99[^|]|%d|%f|%f",
                       &id, name, &qty, &c, &s) == 5) {
                if (qty <= 5) lowCount++;
            }
        }
        fclose(fr);
    }

    float transportCostTotal = 0;
    FILE *ft = fopen("transport.txt", "r");
    if (ft) {
        while (fgets(line, sizeof(line), ft)) {
            int tid;
            char vehicle[NAME_LEN], driver[NAME_LEN], src[NAME_LEN], dst[NAME_LEN], status[30];
            float cost;

            if (sscanf(line, "%d|%99[^|]|%99[^|]|%99[^|]|%99[^|]|%f|%29[^\n]",
                       &tid, vehicle, driver, src, dst, &cost, status) == 7) {
                if (strcmp(status, "Delivered") == 0)
                    transportCostTotal += cost;
            }
        }
        fclose(ft);
    }

    printf("\n--- SUMMARY REPORT ---\n");
    printf("Total Sales: %.2f\n", totalSales);
    printf("Total Profit: %.2f\n", totalProfit);
    printf("Transport Cost (Delivered only): %.2f\n", transportCostTotal);
    printf("Retail low-stock items (<=5 qty): %d\n", lowCount);
}

/*---------------- Menus ----------------*/
void supplier_menu() {
    int ch;
    do {
        printf("\n--- Supplier Menu ---\n");
        printf("1. Add Product\n");
        printf("2. View Supplier Stock\n");
        printf("3. Back\n");
        printf("Choice: ");

        if (scanf("%d", &ch) != 1) { clear_input_buffer(); ch = 3; }
        clear_input_buffer();

        switch (ch) {
            case 1: add_supplier_product(); break;
            case 2: view_suppliers(); break;
            case 3: break;
            default: printf("Invalid choice.\n");
        }
    } while (ch != 3);
}

void wholesaler_menu() {
    int ch;
    do {
        printf("\n--- Wholesaler Menu ---\n");
        printf("1. View Supplier Stock\n");
        printf("2. Receive from Supplier\n");
        printf("3. View Wholesaler Stock\n");
        printf("4. Back\n");
        printf("Choice: ");

        if (scanf("%d", &ch) != 1) { clear_input_buffer(); ch = 4; }
        clear_input_buffer();

        switch (ch) {
            case 1: view_suppliers(); break;
            case 2: wholesaler_receive_from_supplier(); break;
            case 3: view_wholesale_stock(); break;
            case 4: break;
            default: printf("Invalid choice.\n");
        }
    } while (ch != 4);
}

void retailer_menu() {
    int ch;
    do {
        printf("\n--- Retailer Menu ---\n");
        printf("1. View Wholesale Stock\n");
        printf("2. Receive from Wholesaler\n");
        printf("3. View Retail Stock\n");
        printf("4. Sell to Consumer\n");
        printf("5. Back\n");
        printf("Choice: ");

        if (scanf("%d", &ch) != 1) { clear_input_buffer(); ch = 5; }
        clear_input_buffer();

        switch (ch) {
            case 1: view_wholesale_stock(); break;
            case 2: retailer_receive_from_wholesaler(); break;
            case 3: view_retail_stock(); break;
            case 4: generate_bill_for_consumer(); break;
            case 5: break;
            default: printf("Invalid choice.\n");
        }
    } while (ch != 5);
}

void transport_menu() {
    int ch;
    do {
        printf("\n--- Transport Menu ---\n");
        printf("1. Add Transport Record\n");
        printf("2. View Transport Records\n");
        printf("3. Update Transport Status\n");
        printf("4. Back\n");
        printf("Choice: ");

        if (scanf("%d", &ch) != 1) { clear_input_buffer(); ch = 4; }
        clear_input_buffer();

        switch (ch) {
            case 1: add_transport(); break;
            case 2: view_transports(); break;
            case 3: update_transport_status(); break;
            case 4: break;
            default: printf("Invalid choice.\n");
        }
    } while (ch != 4);
}

void reports_menu() {
    int ch;
    do {
        printf("\n--- Reports Menu ---\n");
        printf("1. View Bills\n");
        printf("2. Summary Report\n");
        printf("3. Back\n");
        printf("Choice: ");

        if (scanf("%d", &ch) != 1) { clear_input_buffer(); ch = 3; }
        clear_input_buffer();

        switch (ch) {
            case 1: view_bills(); break;
            case 2: summary_report(); break;
            case 3: break;
            default: printf("Invalid choice.\n");
        }
    } while (ch != 3);
}

void main_menu() {
    int ch;
    do {
        printf("\n====== Market Management System ======\n");
        printf("1. Supplier Module\n");
        printf("2. Wholesaler Module\n");
        printf("3. Retailer & Billing Module\n");
        printf("4. Transport Module\n");
        printf("5. Reports\n");
        printf("6. Exit\n");
        printf("Choose an option: ");

        if (scanf("%d", &ch) != 1) { clear_input_buffer(); ch = 6; }
        clear_input_buffer();

        switch (ch) {
            case 1: supplier_menu(); break;
            case 2: wholesaler_menu(); break;
            case 3: retailer_menu(); break;
            case 4: transport_menu(); break;
            case 5: reports_menu(); break;
            case 6: printf("Exiting. Data saved.\n"); break;
            default: printf("Invalid choice.\n");
        }
    } while (ch != 6);
}

/*---------------- MAIN ----------------*/
int main() {
    printf("Welcome to Market Management System (Text-file based)\n");
    main_menu();
    return 0;
}



