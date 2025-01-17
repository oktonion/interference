/////////////////////////////////////////////////////////////////////////////
// Name:        neuron/entry.cpp
// Purpose:     Neuron entry class
// Author:      Nickolay Babbysh
// Created:     29.04.2019
// Copyright:   (c) NickWare Group
// Licence:     MIT licence
/////////////////////////////////////////////////////////////////////////////

#include "../../include/inn/neuron.h"
#include "../../include/inn/system.h"

inn::Neuron::Entry::Entry() {
    t = 0;
    tm = -1;
    SignalPointer = 0;
    Signal = new float;
    SignalSize = 1;
}

inn::Neuron::Entry::Entry(const Entry &E) {
    for (int64_t i = 0; i < E.getSynapsesCount(); i++) {
        auto *S = new Synapse(*E.getSynapse(i));
        Synapses.push_back(S);
    }
    t = 0;
    tm = -1;
    Signal = new float;
    SignalSize = 1;
    SignalPointer = 0;
}

bool inn::Neuron::Entry::doCheckState(int64_t tn) const {
//    std::cout << "check state " << tm << " " << tn << std::endl;
    return tm >= tn;
}

void inn::Neuron::Entry::doAddSynapse(inn::Position *SPos, unsigned int Xm, float k1, int64_t Tl, int NT) {
	auto *S = new Synapse(SPos, k1, inn::Computer::getLambdaValue(Xm), Tl, NT);
    Synapses.push_back(S);
}

void inn::Neuron::Entry::doIn(float Xt, int64_t tn) {
    if (SignalPointer >= SignalSize) SignalPointer = 0;
    Signal[SignalPointer] = Xt;
    SignalPointer++;
    tm = tn;
//    std::cout << "In entry " << tm << " " << t << " " << SignalPointer << " " << Signal[SignalPointer-1] << " " << Xt << std::endl;
}

void inn::Neuron::Entry::doProcess() {
    auto d = tm - t + 1;
//    std::cout << "Entry " << tm << " " << t << " " << d << " " << SignalPointer-d << " " << Signal[SignalPointer-d] << std::endl;
    if (d > 0 && SignalPointer-d >= 0) {
//        std::cout << SignalPointer-d << std::endl;
        for (auto S: Synapses) {
            if (t >= S->getTl()) S -> doIn(Signal[SignalPointer-d]);
            else S -> doIn(0);
        }
        t++;
    }
}

void inn::Neuron::Entry::doPrepare() {
    t = 0;
    tm = -1;
    for (auto S: Synapses) S -> doReset();
    //for (auto S: Synapses) S -> doPrepare();
}

void inn::Neuron::Entry::doFinalize() {
    //for (auto Sig: Signal)
        //for (auto S: Synapses) S -> doIn(Sig);
    for (auto S: Synapses) S -> doReset();
    //Signal.clear();
}

void inn::Neuron::Entry::doReserveSignalBuffer(uint64_t L) {
    delete [] Signal;
    Signal = new float[L];
    SignalSize = L;
    SignalPointer = 0;
}

void inn::Neuron::Entry::setk1(float _k1) {
    for (auto S: Synapses) S -> setk1(_k1);
}

void inn::Neuron::Entry::setk2(float _k2) {
    for (auto S: Synapses) S -> setk2(_k2);
}

inn::Neuron::Synapse* inn::Neuron::Entry::getSynapse(int64_t SID) const {
    return Synapses[SID];
}

int64_t inn::Neuron::Entry::getSynapsesCount() const {
    return Synapses.size();
}

float inn::Neuron::Entry::getIn() {
    auto d = tm - t + 1;
    if (d > 0 && SignalPointer-d >= 0) {
        t++;
        return Signal[SignalPointer-d];
    }
    return 0;
}

inn::Neuron::Entry::~Entry() {
    for (auto S: Synapses) delete S;
}
