#include <bits/stdc++.h>
using namespace std;

struct Order {
    uint64_t order_id;
    char side;
    double price;
    int size;
};

map<double, int, greater<double>> bid_book;
map<double, int> ask_book;
unordered_map<uint64_t, Order> order_map;

struct TradeRecord {
    char side;
    double price;
    int size;
};
vector<TradeRecord> pending_trades;

typedef vector<pair<double, int>> BookSide;

BookSide get_top_levels(const map<double, int, greater<double>>& book) {
    BookSide top;
    for (map<double, int, greater<double>>::const_iterator it = book.begin(); it != book.end() && top.size() < 10; ++it) {
        if (it->second > 0) top.push_back(*it);
    }
    while (top.size() < 10) top.push_back(make_pair(0.0, 0));
    return top;
}

BookSide get_top_levels(const map<double, int>& book) {
    BookSide top;
    for (map<double, int>::const_iterator it = book.begin(); it != book.end() && top.size() < 10; ++it) {
        if (it->second > 0) top.push_back(*it);
    }
    while (top.size() < 10) top.push_back(make_pair(0.0, 0));
    return top;
}

bool is_same_book(const BookSide& a, const BookSide& b) {
    for (int i = 0; i < 10; ++i) {
        int pa = round(a[i].first * 100);
        int pb = round(b[i].first * 100);
        if (pa != pb || a[i].second != b[i].second) return false;
    }
    return true;
}

string format_snapshot(const string& ts, const BookSide& bids, const BookSide& asks) {
    ostringstream out;
    out << ts;
    out << fixed << setprecision(2);
    for (int i = 0; i < 10; ++i) {
        if (bids[i].first > 0)
            out << "," << bids[i].first << "," << bids[i].second;
        else
            out << ",,";
    }
    for (int i = 0; i < 10; ++i) {
        if (asks[i].first > 0)
            out << "," << asks[i].first << "," << asks[i].second;
        else
            out << ",,";
    }
    return out.str();
}

int emitted_snapshots = 0;

void maybe_emit_snapshot(const string& ts, BookSide& last_bids, BookSide& last_asks) {
    BookSide curr_bids = get_top_levels(bid_book);
    BookSide curr_asks = get_top_levels(ask_book);
    if (!is_same_book(curr_bids, last_bids) || !is_same_book(curr_asks, last_asks)) {
        cout << format_snapshot(ts, curr_bids, curr_asks) << '\n';
        emitted_snapshots++;
        last_bids = curr_bids;
        last_asks = curr_asks;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: ./reconstruction_shard <mbo.csv>\n";
        return 1;
    }

    ifstream file(argv[1]);
    if (!file.is_open()) {
        cerr << "Error opening file: " << argv[1] << "\n";
        return 1;
    }

    string line;
    getline(file, line); // Skip header

    string last_ts = "";
    BookSide last_bids(10, make_pair(0.0, 0)), last_asks(10, make_pair(0.0, 0));

    while (getline(file, line)) {
        stringstream ss(line);
        vector<string> tokens;
        string token;
        while (getline(ss, token, ',')) tokens.push_back(token);

        try {
            if (tokens.size() < 11) continue;

            string ts = tokens[0];
            string action = tokens[5];
            char side = tokens[6].empty() ? 'N' : tokens[6][0];
            double price = tokens[7].empty() ? 0.0 : stod(tokens[7]);
            int size = tokens[8].empty() ? 0 : stoi(tokens[8]);
            uint64_t oid = tokens[10].empty() ? 0 : stoull(tokens[10]);

            if (!last_ts.empty() && ts != last_ts)
                maybe_emit_snapshot(last_ts, last_bids, last_asks);
            last_ts = ts;

            if (action == "A") {
                if (side == 'B' || side == 'A') order_map[oid] = {oid, side, price, size};
                if (side == 'B') bid_book[price] += size;
                else if (side == 'A') ask_book[price] += size;
            }
            else if (action == "M") {
                if (!order_map.count(oid)) continue;
                Order& o = order_map[oid];
                if (o.side == 'B') {
                    bid_book[o.price] -= o.size;
                    if (bid_book[o.price] <= 0) bid_book.erase(o.price);
                    o.price = price; o.size = size;
                    bid_book[price] += size;
                } else {
                    ask_book[o.price] -= o.size;
                    if (ask_book[o.price] <= 0) ask_book.erase(o.price);
                    o.price = price; o.size = size;
                    ask_book[price] += size;
                }
            }
            else if (action == "C") {
                if (order_map.count(oid)) {
                    Order o = order_map[oid];
                    if (o.side == 'B') {
                        bid_book[o.price] -= o.size;
                        if (bid_book[o.price] <= 0) bid_book.erase(o.price);
                    } else {
                        ask_book[o.price] -= o.size;
                        if (ask_book[o.price] <= 0) ask_book.erase(o.price);
                    }
                    order_map.erase(oid);
                } else if (!pending_trades.empty()) {
                    TradeRecord tr = pending_trades.back(); pending_trades.pop_back();
                    char cancel_side = (tr.side == 'A') ? 'B' : 'A';
                    if (cancel_side == 'B') bid_book[tr.price] -= tr.size;
                    else ask_book[tr.price] -= tr.size;
                }
            }
            else if ((action == "T" || action == "F") && side != 'N') {
                pending_trades.push_back({side, price, size});
            }
            else if (action == "R") {
                bid_book.clear(); ask_book.clear();
                order_map.clear(); pending_trades.clear();
                maybe_emit_snapshot(ts, last_bids, last_asks);
                continue;
            }
        } catch (...) {
            continue;
        }
    }

    if (!last_ts.empty()) maybe_emit_snapshot(last_ts, last_bids, last_asks);
    cerr << "Emitted snapshots: " << emitted_snapshots << "\n";
    return 0;
}
