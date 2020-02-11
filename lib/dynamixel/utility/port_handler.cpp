#include "port_handler.h"


DXLPortHandler::DXLPortHandler()
 : open_state_(false)
{}

/* DXLPortHandler */
bool DXLPortHandler::getOpenState()
{
  return open_state_;
}

void DXLPortHandler::setOpenState(bool state)
{
  open_state_ = state;
}

