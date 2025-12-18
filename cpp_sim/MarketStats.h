#pragma once
struct MarketStats {
    double bestBid;
    double bestAsk;
    double midPrice;
    double spread;
    double currentDepthLevel;
    //double currentPriceChange;
    //double currentRelativeSpread;
    double fundamentalValue;
    double volatility;
    double slippage;
};
