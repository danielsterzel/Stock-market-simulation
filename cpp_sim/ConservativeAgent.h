#pragma once

#include "Agent.h"

class ConservativeAgent final:public Agent {
    Order generateAction() override;

};



