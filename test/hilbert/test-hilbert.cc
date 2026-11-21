// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include <stdio.h>

#include "hilbert.h"
#include "chain.h"
#include "signal-source.h"
#include "zmq-sample-pub.h"
#include "timer.h"
#include "menu.h"

constexpr uint32_t 	Fs = 48000;
constexpr uint32_t 	F = 1000;
constexpr size_t	blkSize = Fs / F;

constexpr char const *EPID = "ipc:///tmp/radiomon_sock-hilbert-test";

dsp::endpoints::signal_source_ff sig { blkSize, F, Fs };
bool running = true;

bool menuFreq(const std::string &input);
bool menuQuit(const std::string &input);

int main(int argc, char **argvp)
{
	util::chain theChain("HILBERT_TEST");

	theChain.add(sig, "SIG_SOURCE");
	theChain.add(std::make_unique<dsp::hilbert_default>(), "HILBERT");
	theChain.add(std::make_unique<dsp::zmq_sample_pub_cc_snk>(EPID), "ZMQ_SNK");

	if (!theChain.setup())
	{
		printf("Chain setup failed\n");
		return -1;
	}

	util::menu menu { "Hilbert Test "};
	util::menu::entry entry;

	entry.key = 'F';
	entry.prompt = "Freq";
	entry.description = "Set new frequency (Hz)";
	entry.handler = menuFreq;
	menu.add(entry);

	entry.key = 'q';
	entry.prompt = nullptr;
	entry.description = "Quit";
	entry.handler = menuQuit;
	menu.add(entry);

	menu.set_help(true);
	menu.display();
	while(running)
	{
		theChain.iterate();
		menu.processInput();
		util::timer::sleep(1);
	}

    return 0;
}

bool menuFreq(const std::string &input)
{
	dsp::rate_t rate = static_cast<dsp::rate_t>(stoul(input));

	printf("Current Freq: %u\n", sig.frequency());
	printf("Set freq: %u\n", rate);

	sig.setFrequency(rate);
	return true;
}

bool menuQuit(const std::string &input)
{
	running = false;
	return true;
}