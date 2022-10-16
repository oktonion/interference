/////////////////////////////////////////////////////////////////////////////
// Name:        neuralnet/neuralnet.cpp
// Purpose:     Neural net main class
// Author:      Nickolay Babbysh
// Created:     12.05.2019
// Copyright:   (c) NickWare Group
// Licence:     MIT licence
/////////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <queue>
#include "../../include/inn/neuralnet.h"
#include "../../include/inn/error.h"
#include "../3rdparty/json.hpp"

typedef nlohmann::json json;

inn::NeuralNet::NeuralNet() {
    EntriesCount = 0;
    LDRCounterE = 0;
    LDRCounterN = 0;
    t = 0;
    DataDone = false;
    Learned = false;
}

void inn::NeuralNet::doAddNeuron(Neuron *N, std::vector<inn::LinkDefinition> LinkFromTo) {
    /*
    int i = 0;
    unsigned int ML, MLF = 0;
    inn::NeuralNet::LinkMapRange Range;
    inn::NeuralNet::Link *nL;
    inn::Neuron *oN;
    for (auto L: LinkFromTo) {
        if (i >= N->getEntriesCount()) {
            throw inn::Error(inn::EX_NEURALNET_NEURON_ENTRIES);
        }
        if (std::get<0>(L) == LINK_ENTRY2NEURON && std::get<1>(L) >= EntriesCount) {
            throw inn::Error(inn::EX_NEURALNET_ENTRIES);
        }
        switch (std::get<0>(L)) {
            case LINK_ENTRY2NEURON:
                NeuronLinks.insert(std::pair<inn::Neuron*, inn::NeuralNet::Link>(N, inn::NeuralNet::Link(std::get<0>(L), std::get<1>(L))));
                break;
            case LINK_NEURON2NEURON:
                ML = 0;
                if (std::get<1>(L) == Neurons.size()) oN = N;
                else oN = std::get<2>(Neurons[std::get<1>(L)]);
                Range = NeuronLinks.equal_range(oN);
                for (auto it = Range.first; it != Range.second; ++it)
                    if (it->second.getLatency() > ML) ML = it -> second.getLatency();
                if (ML+1 > MLF) MLF = ML + 1;
                nL = new inn::NeuralNet::Link(std::get<0>(L), oN);
                nL -> setLatency(ML+1);
                NeuronLinks.insert(std::pair<inn::Neuron*, inn::NeuralNet::Link>(N, *nL));
                break;
            default:
                throw inn::Error(inn::EX_NEURALNET_LINKTYPE);
        }
        i++;
    }
    Neurons.emplace_back(Neurons.size(), MLF, N);
     */
}

void inn::NeuralNet::doAddNeuron(Neuron *N, std::vector<inn::LinkDefinitionRange> LinkFromToRange) {
    std::vector<inn::LinkDefinition> LinkFromTo;
    for (auto &LinkRange: LinkFromToRange) {
        for (auto i = std::get<1>(LinkRange); i <= std::get<2>(LinkRange); i++)
            LinkFromTo.emplace_back(std::get<0>(LinkRange), i);
    }
    doAddNeuron(N, LinkFromTo);
}

void inn::NeuralNet::doAddNeuron(Neuron *N, inn::LinkDefinitionRangeNext LinkFromToRangeNext) {
    std::vector<inn::LinkDefinition> LinkFromTo;
    switch (std::get<0>(LinkFromToRangeNext)) {
        case LINK_ENTRY2NEURON:
            for (auto i = LDRCounterE; i < LDRCounterE+std::get<1>(LinkFromToRangeNext); i++)
                LinkFromTo.emplace_back(std::get<0>(LinkFromToRangeNext), i);
            LDRCounterE += std::get<1>(LinkFromToRangeNext);
            break;
        case LINK_NEURON2NEURON:
            for (auto i = LDRCounterN; i < LDRCounterN+std::get<1>(LinkFromToRangeNext); i++)
                LinkFromTo.emplace_back(std::get<0>(LinkFromToRangeNext), i);
            LDRCounterN += std::get<1>(LinkFromToRangeNext);
            break;
        default:
            throw inn::Error(inn::EX_NEURALNET_LINKTYPE);
    }
    doAddNeuron(N, LinkFromTo);
}

void inn::NeuralNet::doCreateNewEntries(unsigned int _EC) {
    EntriesCount = _EC;
}

void inn::NeuralNet::doCreateNewOutput(const std::string& NeuronName) {
    auto N = Neurons.find(NeuronName);
    //if (N != Neurons.end()) Outputs.push_back(N->second);
}

std::vector<double> inn::NeuralNet::doComparePatterns() {
    std::vector<double> PDiffR, PDiffL, PDiff;
    for (auto O: Outputs) {
//        auto P = O -> doComparePattern();
//        PDiffR.push_back(std::get<0>(P));
//        PDiffL.push_back(std::get<1>(P));
    }
    double PDRMin = PDiffR[std::distance(PDiffR.begin(), std::min_element(PDiffR.begin(), PDiffR.end()))];
    double PDRMax = PDiffR[std::distance(PDiffR.begin(), std::max_element(PDiffR.begin(), PDiffR.end()))] - PDRMin;

    double PDLMin = PDiffL[std::distance(PDiffL.begin(), std::min_element(PDiffL.begin(), PDiffL.end()))];
    double PDLMax = PDiffL[std::distance(PDiffL.begin(), std::max_element(PDiffL.begin(), PDiffL.end()))] - PDLMin;

//    for (auto &PDR: PDiffR) {
//        PDR = 1 - (PDR-PDRMin) / PDRMax;
//    }
    for (auto &PDL: PDiffL) {
        PDL = 1 - (PDL-PDLMin) / PDLMax;
        std::cout << PDL << std::endl;
    }
    for (auto i = 0; i < Outputs.size(); i++) {
        PDiff.push_back((PDiffR[i]+PDiffL[i])/2);
    }
    return PDiffR;
}

void inn::NeuralNet::doEnableMultithreading() {
    //for (auto N: Neurons) std::get<2>(N) -> doEnableMultithreading();
}

void inn::NeuralNet::doPrepare() {
//    std::sort(Neurons.begin(), Neurons.end(), [](const inn::NeuralNet::NeuronDefinition &N1, const inn::NeuralNet::NeuronDefinition &N2) -> bool
//    {
//        return std::get<1>(N1) > std::get<1>(N2);
//    });
    for (const auto& N: Neurons) N.second -> doPrepare();
}

void inn::NeuralNet::doFinalize() {
    if (Neurons.empty()) return;
    /*
    std::vector<double> nX;
    unsigned int Tl = std::get<1>(Neurons[0]);
    if (Tl) {
        for (auto i = 0; i < EntriesCount; i++) nX.push_back(0);
        for (auto i = 0; i < Tl; i++) doSignalSend(nX);
        DataDone = true;
        while (true) {
            bool End = true;
            for (auto N: Neurons) {
                if (std::get<1>(N) == Tl) {
                    LinkMapRange R = NeuronLinks.equal_range(std::get<2>(N));
                    for (auto it = R.first; it != R.second; ++it) {
                        //std::cout << it->second.getTime() << std::endl;
                        if ((!Learned && it->second.getTime() != t) || (Learned && it->second.getTime() != t+it->second.getLinkFromE()->getTlo())) {
                            End = false;
                            break;
                        }
                    }
                    if (!End) break;
                }
            }
            if (End) break;
            doSignalSend(nX);
        }
    }
    for (auto N: Neurons) std::get<2>(N) -> doFinalize();
    Learned = true;
     */
}

void inn::NeuralNet::doReset() {
    t = 0;
    DataDone = false;
    //for (auto &NL: NeuronLinks) NL.second.doResetSignalController();
    for (const auto& N: Neurons) N.second -> doReinit();
}

void inn::NeuralNet::doSignalSend(const std::vector<double>& X) {
    std::queue<std::tuple<std::string, std::string, double>> nqueue;
    std::map<std::string, std::string> nwaiting;
    std::vector<std::string> m;

    int xi = 0;
    for (auto &e: Entries) {
        for (auto &en: e.second) {
            nqueue.push(std::make_tuple(e.first, en, X[xi]));
        }
        xi++;
    }

    while (!nqueue.empty()) {
        auto i = nqueue.front();
        nqueue.pop();
        std::cout << std::get<0>(i) << " -> " << std::get<1>(i) << " (" << std::get<2>(i) << ")" << std::endl;
        auto n = Neurons.find(std::get<1>(i));
        if (n != Neurons.end()) {
            if (!t) {
                auto waiting = n -> second -> getWaitingEntries();
                auto l = Latencies.find(std::get<1>(i));
                auto latency = l == Latencies.end() ? 0 : l->second;
                for (auto &we: waiting) {
                    auto nfrom = Latencies.find(we);
                    if (nfrom != Latencies.end() && nfrom->second > latency) {
                        std::cout << we << " (latency " << nfrom->second << ") -> " << std::get<1>(i) << " (0)" << std::endl;
                        n -> second -> doSignalSendEntry(we, 0, {});
                    }
                }
            }

            bool done = n -> second -> doSignalSendEntry(std::get<0>(i), std::get<2>(i), {});

            if (done) {
                std::cout << std::get<1>(i) << " computed" << std::endl;
                auto nlinks = n -> second -> getLinkOutput();
                for (auto &nl: nlinks) {
                    nqueue.push(std::make_tuple(std::get<1>(i), nl, n->second->doSignalReceive()));
                }
            }
        }
    }
    t++;
    /*
    if (EntriesCount != X.size()) {
        throw inn::Error(inn::EX_NEURALNET_INPUT);
    }
    std::vector<inn::WaveDefinition> Waves(Neurons.size());
    for (auto N: Neurons) {
        LinkMapRange R = NeuronLinks.equal_range(std::get<2>(N));
        int64_t i = 0;
        for (auto it = R.first; it != R.second; ++it, i++) {
            if (it->second.getLinkType() == inn::LINK_ENTRY2NEURON) {
                if (DataDone) continue;
                std::get<2>(N) -> doSignalSendEntry(i, X[it->second.getLinkFromEID()], std::vector<inn::WaveDefinition>());
            } else {
                if (!it->second.doCheckSignal()) continue;
                //if (Learned) for (int x = 0; x < Neurons.size(); x++) Waves[x] = std::get<2>(Neurons[x])->getWave();
                std::get<2>(N) -> doSignalSendEntry(i, it->second.getSignal(), Waves);
            }
        }
        if (!std::get<2>(N)->isMultithreadingEnabled()) std::get<2>(N) -> doSignalsSend();
    }
    if (!DataDone) t++;
     */
}

std::vector<double> inn::NeuralNet::doSignalReceive() {
    std::vector<double> nY;
    //for (auto N: Outputs) nY.push_back(N->doSignalReceive());
    return nY;
}

bool inn::NeuralNet::isMultithreadingEnabled() {
    //for (auto N: Neurons) if (!std::get<2>(N)->isMultithreadingEnabled()) return false;
    return true;
}

inn::Neuron* inn::NeuralNet::getNeuron(const std::string& NName) {
    auto N = Neurons.find(NName);
    if (N != Neurons.end()) return N->second;
    return nullptr;
}

uint64_t inn::NeuralNet::getNeuronCount() {
    return Neurons.size();
}

void inn::NeuralNet::setStructure(std::ifstream &Stream) {
    if (!Stream.is_open()) {
        std::cout << "Error opening file" << std::endl;
        return;
    }
    std::string jstr;
    while (!Stream.eof()) {
        std::string rstr;
        getline(Stream, rstr);
        jstr.append(rstr);
    }
    setStructure(jstr);
}

void inn::NeuralNet::setStructure(const std::string &Str) {
    try {
        auto j = json::parse(Str);
        //std::cout << j.dump(4) << std::endl;
        Name = j["name"].get<std::string>();
        Description = j["desc"].get<std::string>();
        Version = j["version"].get<std::string>();

        std::multimap<std::string, std::string> links;
        for (auto &jneuron: j["neurons"].items()) {
            auto nname = jneuron.value()["name"].get<std::string>();
            for (auto &jinputs: jneuron.value()["input_signals"].items()) {
                auto iname = jinputs.value().get<std::string>();
                links.insert(std::make_pair(iname, nname));
            }
        }

        for (auto &jentry: j["entries"].items()) {
            auto ename = jentry.value().get<std::string>();
            std::vector<std::string> elinks;
            auto l = links.equal_range(ename);
            for (auto it = l.first; it != l.second; it++) {
                elinks.push_back(it->second);
                std::cout << ename << " -> " << it->second << std::endl;
            }
            Entries.insert(std::make_pair(ename, elinks));
        }

//        for (auto &l: links) {
//            std::cout << l.first << " - " << l.second << std::endl;
//        }

        for (auto &jneuron: j["neurons"].items()) {
            auto nname = jneuron.value()["name"].get<std::string>();
            auto nsize = jneuron.value()["size"].get<unsigned int>();
            auto ndimensions = jneuron.value()["dimensions"].get<unsigned int>();
            if (jneuron.value()["latency"] != nullptr) {
                auto nlatency = jneuron.value()["latency"].get<int>();
                std::cout << nname << " with latency " << nlatency << std::endl;
                Latencies.insert(std::make_pair(nname, nlatency));
            }
            std::vector<std::string> nentries;
            for (auto &jent: jneuron.value()["input_signals"].items()) {
                nentries.push_back(jent.value().get<std::string>());
            }
            auto *N = new inn::Neuron(nsize, ndimensions, 0, nentries);

            for (auto &jsynapse: jneuron.value()["synapses"].items()) {
                std::vector<double> pos;
                if (ndimensions != jsynapse.value()["position"].size()) {
                    std::cout << "Error: position vector size not equal dimension count" << std::endl;
                    return;
                }
                for (auto &jposition: jsynapse.value()["position"].items()) {
                    pos.push_back(jposition.value().get<double>());
                }
                unsigned int tl = 0;
                if (jneuron.value()["tl"] != nullptr) tl = jsynapse.value()["tl"].get<unsigned int>();
                auto sentryid = jsynapse.value()["entry"].get<unsigned int>();
                auto sentry = jneuron.value()["input_signals"][sentryid];
                N -> doCreateNewSynapse(sentry, pos, tl);
            }
            for (auto &jreceptor: jneuron.value()["receptors"].items()) {
                std::vector<double> pos;
                if (ndimensions != jreceptor.value()["position"].size()) {
                    std::cout << "Error: position vector size not equal dimension count" << std::endl;
                    return;
                }
                for (auto &jposition: jreceptor.value()["position"].items()) {
                    pos.push_back(jposition.value().get<double>());
                }
                if (jreceptor.value()["type"] != nullptr && jreceptor.value()["type"].get<std::string>() == "cluster") {
                    auto rcount = jreceptor.value()["count"].get<unsigned int>();
                    auto rradius = jreceptor.value()["count"].get<unsigned int>();
                    //N -> doCreateNewReceptorCluster(0, 0, rradius, 0);
                } else {
                    N -> doCreateNewReceptor(pos);
                }
            }

            auto l = links.equal_range(nname);
            for (auto it = l.first; it != l.second; it++) {
                N -> doLinkOutput(it->second);
                std::cout << nname << " -> " << it->second << std::endl;
            }

            Neurons.insert(std::make_pair(nname, N));
        }
    } catch (std::exception &e) {
        std::cout << "Error parsing structure: " << e.what() << std::endl;
    }
}

std::string inn::NeuralNet::getStructure() {
    return {};
}

std::string inn::NeuralNet::getName() {
    return Name;
}

std::string inn::NeuralNet::getDescription() {
    return Description;
}

std::string inn::NeuralNet::getVersion() {
    return Version;
}

inn::NeuralNet::~NeuralNet() {
    //for (auto N: Neurons) delete std::get<2>(N);
}
