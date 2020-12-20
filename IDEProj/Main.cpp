//
// chat_client.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <deque>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>


#include "BoostClient.h"
#include "message.hpp"
#include "ConsoleGui.h"
#include <Windows.h>

using boost::asio::ip::tcp;

typedef std::deque<message> message_queue;


int width = 520;
int height = 380;
WNDCLASSEX wndClass;
HWND window;
Client* cl;
ConsoleGui* pGui;

HWND myFieldBtns[10][10];
HWND enFieldBtns[10][10];

bool selectedMy[10][10];
int shipLength = 0;

HWND btnFunctional[6];
HWND listBox;

int cellSize = 20;
int cellPad = 2;
int fieldPadTop = 20;
int myFieldPadLeft = 20;
int enFieldPadLeft = 20;

int fncBtnPadLeft = 10;
int fncBtnPadTop = 10;
int fncBtnSizeW = 70;
int fncBtnSizeH = 40;

int listBoxSizeW = 250;
int listBoxSizeH = 200;

const int myFieldId = 4000;
const int enFieldId = 5000;
const int fncBtn = 6000;
const int lBox = 7000;


std::string selected_item = "";
CHAR buff[1024];

#define buttonsConnected { "Create", "Update", "Join", "Join any", "Quit" }
#define buttonsLobby { "Ready", "Leave", "Quit" }
#define buttonsLobbyUnready { "Uneady", "Leave", "Quit" }
#define buttonsSessionPrep { "Set", "Ready" }
#define buttonsSessionPrepReady { }
#define buttonsSessionMyTurn { "Your turn" }
#define buttonsSessionEnTurn { "Enemy turn" }
#define buttonsSessionEndgame { "Continue" }

bool isActive = true;
bool* lastPressed = nullptr;
char orientation = 'h';

std::string SetShip()
{
   for (int i = 0; i < 10; ++i) {
      for (int j = 0; j < 10; ++j) {
         if (selectedMy[i][j]) {
            std::string ship = "s " + std::to_string(i) + " " + std::to_string(j) + " " + std::to_string(shipLength) + " " + orientation;
            shipLength = 0;
            memset(selectedMy, 0, sizeof(bool) * 100);
            return ship;
         }
      }
   }
   return "";
}

void UpdateList(const char* lList)
{
   SendMessage(listBox, LB_RESETCONTENT, 0, 0);
   std::string lll(lList);
   std::stringstream lobbies;
   lobbies << lList;
   int l;
   while (true) {
      lobbies >> l;
      if (lobbies.good())
         SendMessage(listBox, LB_ADDSTRING, 0, (LPARAM)(LPSTR)(std::to_string(l).c_str()));
      else
         break;
   }

   selected_item = "";
}

void SetFuncBtns(std::initializer_list<LPCSTR> names)
{
   for (auto& b : btnFunctional)
   {
      ShowWindow(b, SW_HIDE);
   }
	for (int i = 0; i < names.size();  ++i)
	{
		if (_countof(btnFunctional) == i)
			break;
      auto name = names.begin() + i;
      ShowWindow(btnFunctional[i], SW_SHOW);
		SetWindowText(btnFunctional[i], *name);
	}
}

void SelectItem()
{
   SendMessage(listBox, LB_GETTEXT, SendMessage(listBox, LB_GETCURSEL, 0, 0), (LPARAM)buff);
   selected_item = buff;
}

void SetListVisibility(bool vis)
{
   ShowWindow(listBox, vis ? SW_SHOW : SW_HIDE);
}

void FuncBtn(int fBtn)
{
   std::string msg;
	switch (cl->state)
	{
	case ClientState::connected:
      switch (fBtn)
      {
      case 0:
         cl->HandleSendMessage("create");
         break;
      case 1:
         cl->HandleSendMessage("list");
         break;
      case 2:
         if (!selected_item.empty()) 
            cl->HandleSendMessage("connect " + selected_item);
         break;
      case 3:
         cl->HandleSendMessage("join");
         break;
      case 4:
         cl->HandleSendMessage("quit");
         isActive = false;
         break;
      }
		break;
	case ClientState::lobby:
      switch (fBtn) {
      case 0:
         if (pGui->lobbyReady)
            cl->HandleSendMessage("unready");
         else
            cl->HandleSendMessage("ready");
         break;
      case 1:
         cl->HandleSendMessage("leave");
         break;
      case 2:
         cl->HandleSendMessage("quit");
         isActive = false;
         break;
      }
		break;
	case ClientState::session:
      switch (cl->gameState)
      {
      case egs::preparation:
         switch (fBtn) {
         case 0:
            msg = SetShip();
            if (!msg.empty())
               cl->HandleSendMessage(msg);
            break;
         case 1:
            if (shipLength == 0)
            {
               cl->HandleSendMessage("ready");
            }
            break;
         }
         break;
      case egs::end:
         switch (fBtn) {
         case 0:
            cl->HandleSendMessage("continue");
            break;
         }
         break;
      }
		break;
	}
}

void SetCell(HWND btn, int state)
{
   LPCSTR imgName = "empty.bmp";
   switch (state)
   {
   case (cell::ship | cell::shot):
      imgName = "ship-shot.bmp";
      break;
   case (cell::ship):
      imgName = "ship.bmp";
      break;
   case (cell::shot):
      imgName = "shot.bmp";
      break;
   case 65536:
      imgName = "ship-prep.bmp";
      break;
   }
   auto img = LoadImage(NULL, imgName, IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE | LR_LOADFROMFILE);
   SendMessage(btn, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)img);
}

bool IsNearLast(int x, int y)
{
   if (orientation == 'h') 
      return &selectedMy[x - 1][y] == lastPressed;

   if (orientation == 'v')
      return &selectedMy[x][y - 1] == lastPressed;

   return false;
}

bool CanSetShip(int x, int y)
{
   if (shipLength <= 1 || (shipLength < 4 && IsNearLast(x, y))) {
      bool noShipsYet = true;
      for (int i = -1; i <= 1; ++i) {
         for (int j = -1; j <= 1; ++j) {
            if (cl->m_my.check_bound(x + i, y + j) && cl->m_my.get_cell(x + i, y + j)) {
               noShipsYet = false;
               i = 2;
               break;
            }
         }
      }
      return noShipsYet;
   }
   return false;
}

void PrepShip(int fBtn)
{
   if (cl->state != ClientState::session && cl->gameState != egs::preparation)
      return;
   int i = fBtn / 10;
   int j = fBtn % 10;

   if (CanSetShip(i, j)) {
      bool& selected = selectedMy[i][j];
      selected = !selected;
      SetCell(myFieldBtns[i][j], selected ? 65536 : 0);
      shipLength++;
      if (i > 0 && &selectedMy[i - 1][j] == lastPressed)
         orientation = 'h';
      else
         orientation = 'v';
      lastPressed = &selected;
   }
}

void Shoot(int fBtn)
{
   if (cl->state != ClientState::session && cl->gameState != egs::my_turn)
      return;

   std::string shot = std::to_string(fBtn / 10) + " " + std::to_string(fBtn % 10);
   cl->HandleSendMessage(shot);

}

long long winProc(HWND window, unsigned msg, WPARAM wp, LPARAM lp)
{
   if (!isActive)
   {
      //system("kill");
      throw(0);
      PostQuitMessage(0);
      return 0;
   }
   auto lwp = LOWORD(wp);
   switch (msg) {
   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   case WM_COMMAND:
      switch (lwp) {
      case lBox:
         if (HIWORD(wp) == LBN_SELCHANGE)
            SelectItem();
         return 0;
		}
		if (LOWORD(wp) >= fncBtn && lwp < fncBtn + _countof(btnFunctional))
		{
			FuncBtn(lwp - fncBtn);
         return 0;
      }
      if (LOWORD(wp) >= myFieldId && lwp < myFieldId + 100) {
         PrepShip(lwp - myFieldId);
         return 0;
      }
      if (LOWORD(wp) >= enFieldId && lwp < enFieldId + 100) {
         Shoot(lwp - enFieldId);
         return 0;
      }
   default:
      return DefWindowProc(window, msg, wp, lp);
   }
}

void InitWindow()
{
	for (int i = 0; i < 10; ++i)
	{
	   for (int j = 0; j < 10; ++j)
	   {
			myFieldBtns[i][j] = CreateWindowA(TEXT("button"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_BITMAP,
				myFieldPadLeft + i * (cellSize + cellPad), fieldPadTop + j * (cellSize + cellPad), cellSize, cellSize,
            window, (HMENU)(myFieldId + i * 10 + j), NULL, NULL);
         SetCell(myFieldBtns[i][j] , 0);

         enFieldBtns[i][j] = CreateWindowA(TEXT("button"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_BITMAP,
            myFieldPadLeft + 10 * (cellSize + cellPad) + enFieldPadLeft + i * (cellSize + cellPad), fieldPadTop + j * (cellSize + cellPad), cellSize, cellSize,
            window, (HMENU)(enFieldId + i * 10 + j), NULL, NULL);
         SetCell(enFieldBtns[i][j], 0);
	   }
	}
	
	for (int i = 0; i < _countof(btnFunctional); ++i)
   {
      btnFunctional[i] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("button"), TEXT(""), WS_CHILD | WS_VISIBLE,
         myFieldPadLeft + i * (fncBtnSizeW + fncBtnPadLeft), fieldPadTop + 10 * (cellSize + cellPad) + fncBtnPadTop, fncBtnSizeW, fncBtnSizeH,
         window, (HMENU)(fncBtn + i), NULL, NULL);
	}

   listBox = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("listbox"), TEXT("Lobbies"), WS_CHILD | WS_VISIBLE | LBS_STANDARD | LBS_NOTIFY,
      myFieldPadLeft, fieldPadTop, listBoxSizeW, listBoxSizeH,
      window, (HMENU)lBox, GetModuleHandle(nullptr), NULL);
}

void SetEnFieldCells(bool visible, bool enabled = false)
{
   for (int i = 0; i < 10; ++i)
   {
      for (int j = 0; j < 10; ++j)
      {
         ShowWindow(enFieldBtns[i][j], visible ? SW_SHOW : SW_HIDE);
         if (visible) {
            SetCell(enFieldBtns[i][j], cl->m_en.get_cell(i, j));
         //EnableWindow(enFieldBtns[i][j], enabled);
         }
      }
   }
}

void SetMyFieldCells(bool visible, bool enabled = false)
{
   for (int i = 0; i < 10; ++i) {
      for (int j = 0; j < 10; ++j) {
         ShowWindow(myFieldBtns[i][j], visible ? SW_SHOW : SW_HIDE);
         if (visible) {
            SetCell(myFieldBtns[i][j], cl->m_my.get_cell(i, j));
         }
         //EnableWindow(myFieldBtns[i][j], enabled);
      }
   }
}

void UpdateWindow()
{
   switch (cl->state)
   {
   case ClientState::connected:
      SetFuncBtns(buttonsConnected);
      SetListVisibility(true);
      SetMyFieldCells(false);
      SetEnFieldCells(false);
      break;
   case ClientState::lobby:
      if (pGui->lobbyReady)
         SetFuncBtns(buttonsLobbyUnready);
      else
         SetFuncBtns(buttonsLobby);
      SetListVisibility(false);
      SetMyFieldCells(false);
      SetEnFieldCells(false);
      break;
   case ClientState::session:
      SetListVisibility(false);
      switch (cl->gameState)
      {
      case egs::preparation:
         SetFuncBtns(buttonsSessionPrep);
         SetMyFieldCells(true, true);
         SetEnFieldCells(false);
         break;
      case egs::my_turn:
         SetFuncBtns(buttonsSessionMyTurn);
         SetMyFieldCells(true, false);
         SetEnFieldCells(true, true);
         break;
      case egs::enemy_turn:
         SetFuncBtns(buttonsSessionEnTurn);
         SetMyFieldCells(true, false);
         SetEnFieldCells(true, false);
         break;
      case egs::end:
         SetFuncBtns(buttonsSessionEndgame);
         SetMyFieldCells(true);
         SetEnFieldCells(true);
         break;
      }
      break;
   }
}

int main(int argc, char* argv[])
{
   try
   {
      if (argc < 3)
      {
         std::cerr << "Usage: chat_client <host> <port>\n";
         return 1;
      }

      boost::asio::io_service io_service;

      tcp::resolver resolver(io_service);
      tcp::resolver::query query(argv[1], argv[2]);
      tcp::resolver::iterator iterator = resolver.resolve(query);

      BoostClient client(io_service, iterator);
      cl = &client;
      ConsoleGui gui(&client);
      pGui = &gui;

      boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));

      char line[message::max_body_length + 1];
      gui.draw();

      wndClass = { sizeof(WNDCLASSEX), CS_DBLCLKS, winProc,
   0, 0, GetModuleHandle(nullptr), LoadIcon(nullptr,IDI_APPLICATION),
   LoadCursor(nullptr,IDC_ARROW), HBRUSH(COLOR_WINDOW),
   nullptr, "Seafight",LoadIcon(nullptr,IDI_APPLICATION)
      };
      if (RegisterClassEx(&wndClass)) {
         window = CreateWindowEx(0, "Seafight", "Seafight", (WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX), 0, 0,
            width, height, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);
         if (window) {
            ShowWindow(window, SW_SHOWDEFAULT);
            UpdateWindow(window);
         }
      }

      if (window) {
         if (argc != 4) {
            ShowWindow(::GetConsoleWindow(), SW_HIDE);
         }
         else {
            ShowWindow(::GetConsoleWindow(), SW_SHOW);
         }
         MSG msg;
         InitWindow();
         gui.draw();
         while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
         }
      }
      else
      {
         while (std::cin.getline(line, message::max_body_length + 1) && isActive) {
            // we want to check our input if we are't waiting for server message
            client.HandleSendMessage(line);
         }
      }

      client.close();
      t.join();
   }
   catch (std::exception & e)
   {
      std::cerr << "Exception: " << e.what() << "\n";
   }

   return 0;
}