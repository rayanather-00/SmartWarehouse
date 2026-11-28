
#include <iostream>
#include "warehouse.cpp"
#include "Inventory.cpp"
#include "file_handler.cpp"
using namespace std;

int main() {
    warehouse w;
    inventory inv;
    warehouse_data data;

    cout << "========== LOADING DATA ==========\n";

    if (w.searchLocation("L01") != nullptr)
    cout << "L01 found in warehouse.\n";
else
    cout << "L01 NOT found in warehouse.\n";

    if (!inv.load_inventory(data, w)) {
        cout << "Inventory loading failed.\n";
        clear_warehouse_data(data);
        return 1;
    }

    cout << "Inventory loaded successfully.\n";

    // ADD RACK TEST
    cout << "\n--- ADD RACK TEST ---\n";
    if (inv.add_rack("P102", "TEST_RACK", "L01", 0, w))
        cout << "Rack added successfully.\n";
    else
        cout << "Rack addition failed.\n";

    // UPDATE QUANTITY TEST
    cout << "\n--- UPDATE QUANTITY TEST ---\n";
    if (inv.update_quantity("P102", "TEST_RACK", 5))
        cout << "Rack quantity updated to 5.\n";
    else
        cout << "Quantity update failed.\n";

    // REMOVE RACK TEST
    cout << "\n--- REMOVE RACK TEST ---\n";
    inv.update_quantity("P102", "TEST_RACK", 0);

    if (inv.remove_rack("P102", "TEST_RACK"))
        cout << "Rack removed successfully.\n";
    else
        cout << "Rack removal failed.\n";

    // RESERVE STOCK TEST
    cout << "\n--- RESERVE STOCK TEST ---\n";
    if (inv.allocate_nearest_rack("P101", "L01", 3, "TEST_ORDER", w))
        cout << "3 units reserved successfully.\n";
    else
        cout << "Stock reservation failed.\n";

    cout << "Available stock: " << inv.get_available_stock("P101") << endl;

    // RELEASE RESERVATION TEST
    cout << "\n--- RELEASE RESERVATION TEST ---\n";
    if (inv.release_reservations("TEST_ORDER"))
        cout << "Reservations released successfully.\n";
    else
        cout << "Reservation release failed.\n";

    cout << "Available stock: " << inv.get_available_stock("P101") << endl;

    // DEDUCT RESERVED STOCK TEST
    cout << "\n--- DEDUCT RESERVED STOCK TEST ---\n";

    if (inv.allocate_nearest_rack("P101", "L01", 3, "TEST_ORDER", w))
        cout << "Stock reserved for deduction.\n";

    if (inv.deduct_reserved_stock("P101", "TEST_ORDER", 2))
        cout << "2 reserved units deducted successfully.\n";
    else
        cout << "Stock deduction failed.\n";

    // CLEANUP TEST RESERVATIONS AND RESTORE STOCK
    inv.release_reservations("TEST_ORDER");
    inv.update_quantity("P101", "RK01", 10);

    cout << "\n--- FINAL STOCK ---\n";
    cout << "Total stock: " << inv.get_total_stock("P101") << endl;
    cout << "Available stock: " << inv.get_available_stock("P101") << endl;

    // SAVE DATA
    cout << "\n--- SAVING INVENTORY ---\n";

    if (inv.save_inventory(data))
        cout << "Inventory saved successfully.\n";
    else
        cout << "Inventory saving failed.\n";

    cout << "\n========== MODULE 2 TESTING FINISHED ==========\n";

    clear_warehouse_data(data);
    return 0;
}
