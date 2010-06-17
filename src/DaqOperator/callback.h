// -*- C++ -*-

#ifndef CALLBACK_H
#define CALLBACK_H

static int cb_command_configure() {
	DaqOperator* daq = DaqOperator::Instance();
	
	std::string body = daq->getBody();
	///daq->parse_body(body.c_str());
	daq->parse_body(body.c_str(), "params");
	daq->command_configure();
	g_server->setMsg(daq->getMsg());
	return 0;
}
static int cb_command_unconfigure() {
	DaqOperator* daq = DaqOperator::Instance();
	daq->command_unconfigure();
	g_server->setMsg(daq->getMsg());
	return 0;
}
static int cb_command_start() {
	DaqOperator* daq = DaqOperator::Instance();

	std::string body = daq->getBody();
	if ( !daq->parse_body(body.c_str(), "runNo") ) {
	    return -1;
	}

	daq->command_start();
	g_server->setMsg(daq->getMsg());
	return 0;
}
static int cb_command_stop() {
	DaqOperator* daq = DaqOperator::Instance();
	daq->command_stop();
	g_server->setMsg(daq->getMsg());
	return 0;
}
/// obsolete command at MLF
/* 
static int cb_command_abort() {
	DaqOperator* daq = DaqOperator::Instance();
	daq->command_abort();
	g_server->setMsg(daq->getMsg());
	return 0;
}
*/
static int cb_command_confirmend() {
	DaqOperator* daq = DaqOperator::Instance();
	daq->command_confirmend();
	g_server->setMsg(daq->getMsg());
	return 0;
}
/// obsolete command at MLF
/*
static int cb_command_putparams() {
	DaqOperator* daq = DaqOperator::Instance();
	daq->command_putparams();
	g_server->setMsg(daq->getMsg());
	return 0;
}
*/
static int cb_command_putstatus() {
	DaqOperator* daq = DaqOperator::Instance();
	daq->command_putstatus();
	g_server->setMsg(daq->getMsg());
	return 0;
}
static int cb_command_log() {
	DaqOperator* daq = DaqOperator::Instance();
	daq->command_log();
	g_server->setMsg(daq->getMsg());
	return 0;
}
static int cb_command_pause() {
	DaqOperator* daq = DaqOperator::Instance();
	daq->command_pause();
	g_server->setMsg(daq->getMsg());
	return 0;
}
static int cb_command_resume() {
	DaqOperator* daq = DaqOperator::Instance();
	daq->command_resume();
	g_server->setMsg(daq->getMsg());
	return 0;
}
static int cb_command_stopparamsset() {
	DaqOperator* daq = DaqOperator::Instance();
	daq->command_stopparamsset();
	g_server->setMsg(daq->getMsg());
	return 0;
}
/// obsolete command at MLF
/*
static int cb_command_resetparams() {
	DaqOperator* daq = DaqOperator::Instance();
	daq->command_resetparams();
	g_server->setMsg(daq->getMsg());
	return 0;
}
*/
static int cb_command_save() {
	DaqOperator* daq = DaqOperator::Instance();
	daq->command_save();
	g_server->setMsg(daq->getMsg());
	return 0;
}
static int cb_command_confirmconnection() {
	DaqOperator* daq = DaqOperator::Instance();
	daq->command_confirmconnection();
	g_server->setMsg(daq->getMsg());
	return 0;
}
static int cb_command_dummy() {
	DaqOperator* daq = DaqOperator::Instance();
	daq->command_dummy();
	g_server->setMsg(daq->getMsg());
	return 0;
}

#endif
