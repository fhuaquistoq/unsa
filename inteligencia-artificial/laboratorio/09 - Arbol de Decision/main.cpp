#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

struct Record {
    string id;
    string edad;
    string miembro;
    string visito;
    string ingresos;
    string compra;
};

struct Node {
    int id = 0;
    bool isLeaf = false;
    bool isRoot = false;
    string splitAttribute;
    string prediction;
    double entropy = 0.0;
    int subsetSize = 0;
    int countSI = 0;
    int countNO = 0;
    vector<pair<string, Node*>> children;
};

static const vector<string> ATTRIBUTE_ORDER = {"Edad", "Miembro", "Visito", "Ingresos"};
static const map<string, vector<string>> ATTRIBUTE_VALUES = {
    {"Edad", {"Joven", "Adulto", "AdultoMayor"}},
    {"Miembro", {"Standard", "Premium", "VIP"}},
    {"Visito", {"Si", "No"}},
    {"Ingresos", {"Bajo", "Alto"}},
};

string getAttributeValue(const Record& record, const string& attribute) {
    if (attribute == "Edad") return record.edad;
    if (attribute == "Miembro") return record.miembro;
    if (attribute == "Visito") return record.visito;
    return record.ingresos;
}

bool isKnownAttributeValue(const string& attribute, const string& value) {
    const auto it = ATTRIBUTE_VALUES.find(attribute);
    if (it == ATTRIBUTE_VALUES.end()) {
        return false;
    }

    const auto& values = it->second;
    return find(values.begin(), values.end(), value) != values.end();
}

pair<int, int> classCounts(const vector<Record>& subset) {
    int si = 0;
    int no = 0;
    for (const auto& record : subset) {
        if (record.compra == "SI") {
            ++si;
        } else {
            ++no;
        }
    }
    return {si, no};
}

double entropyFromCounts(int si, int no) {
    const int total = si + no;
    if (total == 0 || si == 0 || no == 0) {
        return 0.0;
    }

    const double pSi = static_cast<double>(si) / total;
    const double pNo = static_cast<double>(no) / total;
    return -(pSi * log2(pSi) + pNo * log2(pNo));
}

double entropyOfSubset(const vector<Record>& subset) {
    auto [si, no] = classCounts(subset);
    return entropyFromCounts(si, no);
}

string majorityClass(const vector<Record>& subset, const string& fallback) {
    auto [si, no] = classCounts(subset);
    if (si == no) {
        return fallback;
    }
    return (si > no) ? "SI" : "NO";
}

string formatDouble(double value) {
    ostringstream out;
    out << fixed << setprecision(4) << value;
    return out.str();
}

string indentText(int depth) {
    return string(depth * 4, ' ');
}

vector<Record> splitSubset(const vector<Record>& subset, const string& attribute, const string& value) {
    vector<Record> result;
    for (const auto& record : subset) {
        if (getAttributeValue(record, attribute) == value) {
            result.push_back(record);
        }
    }
    return result;
}

int globalNodeId = 0;

Node* buildTree(const vector<Record>& subset,
                const vector<string>& attributes,
                const string& defaultClass,
                int depth,
                const string& branchLabel) {
    Node* node = new Node();
    node->id = globalNodeId++;
    node->subsetSize = static_cast<int>(subset.size());
    auto [countSI, countNO] = classCounts(subset);
    node->countSI = countSI;
    node->countNO = countNO;
    node->entropy = entropyFromCounts(countSI, countNO);
    node->prediction = subset.empty() ? defaultClass : majorityClass(subset, defaultClass);

    const string indent = indentText(depth);
    cout << indent << "------------------------------------------------------------\n";
    cout << indent << "Nodo " << node->id;
    if (!branchLabel.empty()) {
        cout << " (rama: " << branchLabel << ")";
    }
    cout << "\n";
    cout << indent << "Profundidad: " << depth << "\n";
    cout << indent << "Tamano del subconjunto: " << node->subsetSize << "\n";
    cout << indent << "Conteo de clases -> SI: " << node->countSI << ", NO: " << node->countNO << "\n";
    cout << indent << "Entropia del nodo: " << formatDouble(node->entropy) << "\n";

    if (subset.empty()) {
        node->isLeaf = true;
        cout << indent << "Nodo hoja: subconjunto vacio -> clase por defecto " << node->prediction << "\n";
        return node;
    }

    if (countSI == 0 || countNO == 0) {
        node->isLeaf = true;
        cout << indent << "Nodo hoja: subconjunto puro -> clase " << node->prediction << "\n";
        return node;
    }

    if (attributes.empty()) {
        node->isLeaf = true;
        cout << indent << "Nodo hoja: no quedan atributos -> clase mayoritaria " << node->prediction << "\n";
        return node;
    }

    double bestGain = -1.0;
    string bestAttribute;

    cout << indent << "Evaluacion de atributos candidatos:\n";

    for (const auto& attribute : attributes) {
        cout << indent << "  * Atributo: " << attribute << "\n";
        double weightedEntropy = 0.0;
        for (const auto& value : ATTRIBUTE_VALUES.at(attribute)) {
            vector<Record> childSubset = splitSubset(subset, attribute, value);
            auto [childSI, childNO] = classCounts(childSubset);
            double childEntropy = entropyFromCounts(childSI, childNO);
            double weight = subset.empty() ? 0.0 : static_cast<double>(childSubset.size()) / subset.size();
            const double weightedChildEntropy = weight * childEntropy;
            weightedEntropy += weightedChildEntropy;

            cout << indent << "      - Valor " << value
                 << " -> tamano=" << childSubset.size()
                 << ", SI=" << childSI
                 << ", NO=" << childNO
                 << ", entropia=" << formatDouble(childEntropy)
                 << ", peso=" << formatDouble(weight)
                 << ", entropia ponderada=" << formatDouble(weightedChildEntropy)
                 << "\n";
        }

        const double gain = node->entropy - weightedEntropy;
        cout << indent << "    Entropia ponderada total de " << attribute << ": " << formatDouble(weightedEntropy) << "\n";
        cout << indent << "    Ganancia de informacion de " << attribute << ": " << formatDouble(gain) << "\n";

        if (gain > bestGain + 1e-12) {
            bestGain = gain;
            bestAttribute = attribute;
        }
    }

    node->splitAttribute = bestAttribute;
    cout << indent << "Mejor atributo elegido: " << bestAttribute
         << " (ganancia=" << formatDouble(bestGain) << ")\n";

    vector<string> remainingAttributes;
    for (const auto& attribute : attributes) {
        if (attribute != bestAttribute) {
            remainingAttributes.push_back(attribute);
        }
    }

    for (const auto& value : ATTRIBUTE_VALUES.at(bestAttribute)) {
        vector<Record> childSubset = splitSubset(subset, bestAttribute, value);
        Node* child = buildTree(childSubset, remainingAttributes, node->prediction, depth + 1, bestAttribute + "=" + value);
        node->children.push_back({value, child});
    }

    return node;
}

void printDotNode(const Node* node) {
    const string fillColor = node->isRoot ? "#d9ecff" : (node->isLeaf ? "#d8f3dc" : "#ffe8b6");
    ostringstream label;
    label << "\tNodo " << node->id << "\n";
    if (node->isLeaf) {
        label << "\tClase: " << node->prediction << "\n";
    } else {
        label << "\tDecision: " << node->splitAttribute << "\n";
    }
    label << "\tEntropia: " << formatDouble(node->entropy) << "\n";
    label << "\tn=" << node->subsetSize << " | SI=" << node->countSI << " | NO=" << node->countNO;

    cout << "  node" << node->id
         << " [label=\"" << label.str() << "\", fillcolor=\"" << fillColor << "\"];\n";

    for (const auto& [edgeValue, child] : node->children) {
        printDotNode(child);
        cout << "  node" << node->id << " -> node" << child->id
             << " [label=\"" << edgeValue << "\"];\n";
    }
}

void printDotGraph(const Node* root) {
    cout << "\n============================================================\n";
    cout << "digraph ID3Tree {\n";
    cout << "  rankdir=TB;\n";
    cout << "  graph [bgcolor=white, nodesep=0.4, ranksep=0.6, pad=0.2];\n";
    cout << "  node [shape=box, style=\"rounded,filled\", color=\"#5b5b5b\", fontname=\"Helvetica\", fontsize=10];\n";
    cout << "  edge [fontname=\"Helvetica\", fontsize=10, color=\"#555555\"];\n";
    printDotNode(root);
    cout << "}\n";
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    vector<Record> data;
    string line;
    bool firstLine = true;

    while (getline(cin, line)) {
        if (line.empty()) {
            continue;
        }

        istringstream input(line);
        vector<string> tokens;
        string token;
        while (input >> token) {
            tokens.push_back(token);
        }

        if (tokens.empty()) {
            continue;
        }

        if (firstLine) {
            firstLine = false;
            if (!tokens.empty() && (tokens[0] == "ID" || tokens[0] == "id")) {
                continue;
            }
        }

        if (tokens.size() != 6) {
            cerr << "Registro invalido: se esperaban exactamente 6 columnas, pero llegaron " << tokens.size()
                 << " en la linea: " << line << "\n";
            return 1;
        }

        Record record;
        record.id = tokens[0];
        record.edad = tokens[1];
        record.miembro = tokens[2];
        record.visito = tokens[3];
        record.ingresos = tokens[4];
        record.compra = tokens[5];

        if (!isKnownAttributeValue("Edad", record.edad)) {
            cerr << "Valor invalido de Edad en el registro ID " << record.id << ": " << record.edad << "\n";
            return 1;
        }

        if (!isKnownAttributeValue("Miembro", record.miembro)) {
            cerr << "Valor invalido de Miembro en el registro ID " << record.id << ": " << record.miembro << "\n";
            return 1;
        }

        if (!isKnownAttributeValue("Visito", record.visito)) {
            cerr << "Valor invalido de Visito en el registro ID " << record.id << ": " << record.visito << "\n";
            return 1;
        }

        if (!isKnownAttributeValue("Ingresos", record.ingresos)) {
            cerr << "Valor invalido de Ingresos en el registro ID " << record.id << ": " << record.ingresos << "\n";
            return 1;
        }

        if (record.compra != "SI" && record.compra != "NO") {
            cerr << "Clase invalida en el registro ID " << record.id << ": " << record.compra << "\n";
            return 1;
        }

        data.push_back(record);
    }

    if (data.empty()) {
        cerr << "No se recibieron datos por stdin.\n";
        return 1;
    }

    cout << "REPORTE DEL ARBOL DE DECISION ID3\n";
    cout << "Total de registros cargados: " << data.size() << "\n";
    cout << "Atributos usados para aprender: Edad, Miembro, Visito, Ingresos\n";
    cout << "Clase objetivo: Compra\n";
    cout << "Al final se imprime el bloque DOT para Graphviz.\n\n";

    Node* root = buildTree(data, ATTRIBUTE_ORDER, "SI", 0, "");
    root->isRoot = true;
    printDotGraph(root);
    return 0;
}
