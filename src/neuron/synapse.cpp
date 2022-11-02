/////////////////////////////////////////////////////////////////////////////
// Name:        neuron/synaps.cpp
// Purpose:     Neuron synaps class
// Author:      Nickolay Babbysh
// Created:     29.04.2019
// Copyright:   (c) NickWare Group
// Licence:     MIT licence
/////////////////////////////////////////////////////////////////////////////

#include "../../include/inn/neuron.h"
#include "../../include/inn/system.h"

inn::Neuron::Synapse::Synapse() {
    SPos = new inn::Position();
    ok1 = 0;
    ok2 = 0;
    k1 = 0;
    k2 = 0;
    Lambda = 0;
    Tl = 0;
    WTs = inn::WaveType::NOWAVE;
    Gamma = 0;
    dGamma = 0;
    QCounter = -1;
    QSize = 0;
}

inn::Neuron::Synapse::Synapse(const Synapse &S) {
    SPos = S.getPos();
    ok1 = S.getk1();
    ok2 = S.getk2();
    k1 = ok1;
    k2 = ok2;
    Lambda = S.getLambda();
    Tl = S.getTl();
    WTs = S.getWTs();
    Gamma = S.getGamma();
    dGamma = S.getdGamma();
    QCounter = -1;
    QSize = 0;
}

inn::Neuron::Synapse::Synapse(inn::Position *_SPos, double _k1, double _Lambda, int64_t _Tl, int NT) {
    SPos = _SPos;
    ok1 = _k1;
    ok2 = ok1 * 9e-1;
    k1 = ok1;
	k2 = ok2;
    Lambda = _Lambda;
    Tl = _Tl;
    WTs = inn::WaveType::NOWAVE;
    Gamma = 0;
    dGamma = 0;
    QCounter = -1;
    QSize = 0;
    NeurotransmitterType = NT;
}

void inn::Neuron::Synapse::doIn(double X) {
    double nGamma = ComputeBackend->getGammaFunctionValue(Gamma, k1, k2, X);
    dGamma = nGamma - Gamma;
    Gamma = nGamma;
}

void inn::Neuron::Synapse::doSendToQueue(double X, double WVSum) {
//    double GammaLast = !QSize ? 0 : GammaQ[QSize-1];
//    double nGamma = ComputeBackend->getGammaFunctionValue(GammaLast, k1, k2, X);
//    GammaQ[QSize] = nGamma;
//    QSize++;
}

bool inn::Neuron::Synapse::doInFromQueue(int64_t tT) {
    if (tT >= QSize) return false;
    if (tT == QCounter) return true;
    double nGamma = GammaQ[tT];
    dGamma = nGamma - Gamma;
    Gamma = nGamma;
    QCounter = tT;
    return true;
}

void inn::Neuron::Synapse::doPrepare() {
    ok1 = k1;
    ok2 = k2;
}

void inn::Neuron::Synapse::doReset() {
    Gamma = 0;
    dGamma = 0;
    QCounter = 0;
    QSize = 0;
    GammaQ.clear();
    k1 = ok1;
    k2 = ok2;
}

void inn::Neuron::Synapse::setk1(double _k1) {
    k1 = _k1;
}

void inn::Neuron::Synapse::setk2(double _k2) {
    k2 = _k2;
}

void inn::Neuron::Synapse::setLambda(double L) {
    Lambda = L;
}

void inn::Neuron::Synapse::setWTs(inn::WaveType _WTs) {
    WTs = _WTs;
}

inn::Position* inn::Neuron::Synapse::getPos() const {
    return SPos;
}

double inn::Neuron::Synapse::getk1() const {
    return k1;
}

double inn::Neuron::Synapse::getk2() const {
    return k2;
}

double inn::Neuron::Synapse::getLambda() const {
    return Lambda;
}

int64_t inn::Neuron::Synapse::getTl() const {
    return Tl;
}

inn::WaveType inn::Neuron::Synapse::getWTs() const {
    return WTs;
}

double inn::Neuron::Synapse::getGamma() const {
    return Gamma;
}

double inn::Neuron::Synapse::getdGamma() const {
    return dGamma;
}

int64_t inn::Neuron::Synapse::getQSize() {
    return QSize;
}

int inn::Neuron::Synapse::getNeurotransmitterType() const {
    return NeurotransmitterType;
}
