(*
 * Hedgewars, a free turn based strategy game
 * Copyright (c) 2004-2008 Andrey Korotaev <unC0Rr@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 *)

function CheckNoTeamOrHH: boolean;
var Result: boolean;
begin
Result:= (CurrentTeam = nil) or (CurrentHedgehog^.Gear = nil);
{$IFDEF DEBUGFILE}
if Result then
   if CurrentTeam = nil then AddFileLog('CONSOLE: CurTeam = nil')
                        else AddFileLog('CONSOLE: CurTeam <> nil, Gear = nil');
{$ENDIF}
CheckNoTeamOrHH:= Result
end;
////////////////////////////////////////////////////////////////////////////////
procedure chQuit(var s: shortstring);
const prevGState: TGameState = gsConfirm;
begin
if GameState <> gsConfirm then
        begin
        prevGState:= GameState;
        GameState:= gsConfirm
        end else
        GameState:= prevGState
end;

procedure chConfirm(var s: shortstring);
begin
if GameState = gsConfirm then
	begin
	SendIPC('Q');
	GameState:= gsExit
	end
end;

procedure chCheckProto(var s: shortstring);
var i, c: LongInt;
begin
if isDeveloperMode then
   begin
   val(s, i, c);
   if (c <> 0) or (i = 0) then exit;
   TryDo(i <= cNetProtoVersion, 'Protocol version mismatch: engine is too old', true);
   TryDo(i >= cNetProtoVersion, 'Protocol version mismatch: engine is too new', true)
   end
end;

procedure chAddTeam(var s: shortstring);
var Color: Longword;
    ts: shortstring;
begin
if isDeveloperMode then
   begin
   SplitBySpace(s, ts);
   val(s, Color);
   TryDo(Color <> 0, 'Error: black team color', true);

   Color:= Color or $FF000000;

   AddTeam(Color);
   CurrentTeam^.TeamName:= ts;
   if GameType in [gmtDemo, gmtSave] then CurrentTeam^.ExtDriven:= true
   end
end;

procedure chTeamLocal(var s: shortstring);
begin
if not isDeveloperMode then exit;
if CurrentTeam = nil then OutError(errmsgIncorrectUse + ' "/rdriven"', true);
CurrentTeam^.ExtDriven:= true
end;

procedure chGrave(var s: shortstring);
begin
if CurrentTeam = nil then OutError(errmsgIncorrectUse + ' "/grave"', true);
if s[1]='"' then Delete(s, 1, 1);
if s[byte(s[0])]='"' then Delete(s, byte(s[0]), 1);
CurrentTeam^.GraveName:= s
end;

procedure chFort(var s: shortstring);
begin
if CurrentTeam = nil then OutError(errmsgIncorrectUse + ' "/fort"', true);
if s[1]='"' then Delete(s, 1, 1);
if s[byte(s[0])]='"' then Delete(s, byte(s[0]), 1);
CurrentTeam^.FortName:= s
end;

procedure chAddHH(var id: shortstring);
var s: shortstring;
    Gear: PGear;
begin
if (not isDeveloperMode) or (CurrentTeam = nil) then exit;
with CurrentTeam^ do
	begin
	SplitBySpace(id, s);
	CurrentHedgehog:= @Hedgehogs[HedgehogsNumber];
	val(id, CurrentHedgehog^.BotLevel);
	Gear:= AddGear(0, 0, gtHedgehog, 0, _0, _0, 0);
	SplitBySpace(s, id);
	val(s, Gear^.Health);
	TryDo(Gear^.Health > 0, 'Invalid hedgehog health', true);
	PHedgehog(Gear^.Hedgehog)^.Team:= CurrentTeam;
	CurrentHedgehog^.AmmoStore:= TeamsCount - 1; // FIXME HACK to get ammostores work
	CurrentHedgehog^.Gear:= Gear;
	CurrentHedgehog^.Name:= id;
	inc(HedgehogsNumber)
	end
end;

procedure chSetHat(var s: shortstring);
begin
if (not isDeveloperMode) or (CurrentTeam = nil) then exit;
with CurrentTeam^ do
	if s = '' then
		CurrentHedgehog^.Hat:= 'NoHat'
	else
		CurrentHedgehog^.Hat:= s
end;

procedure chSetHHCoords(var x: shortstring);
var y: shortstring;
    t: Longint;
begin
if (not isDeveloperMode) or (CurrentHedgehog = nil) or (CurrentHedgehog^.Gear = nil) then exit;
SplitBySpace(x, y);
val(x, t);
CurrentHedgehog^.Gear^.X:= int2hwFloat(t);
val(y, t);
CurrentHedgehog^.Gear^.Y:= int2hwFloat(t)
end;

procedure chAddAmmoStore(var descr: shortstring);
begin
AddAmmoStore(descr)
end;

procedure chBind(var id: shortstring);
var s: shortstring;
    b: LongInt;
begin
if CurrentTeam = nil then exit;
SplitBySpace(id, s);
if s[1]='"' then Delete(s, 1, 1);
if s[byte(s[0])]='"' then Delete(s, byte(s[0]), 1);
b:= KeyNameToCode(id);
if b = 0 then OutError(errmsgUnknownVariable + ' "' + id + '"', false)
         else CurrentTeam^.Binds[b]:= s
end;

procedure chLeft_p(var s: shortstring);
begin
if CheckNoTeamOrHH then exit;
bShowFinger:= false;
if not CurrentTeam^.ExtDriven then SendIPC('L');
with CurrentHedgehog^.Gear^ do
    Message:= Message or gm_Left
end;

procedure chLeft_m(var s: shortstring);
begin
if CheckNoTeamOrHH then exit;
if not CurrentTeam^.ExtDriven then SendIPC('l');
with CurrentHedgehog^.Gear^ do
     Message:= Message and not gm_Left
end;

procedure chRight_p(var s: shortstring);
begin
if CheckNoTeamOrHH then exit;
bShowFinger:= false;
if not CurrentTeam^.ExtDriven then SendIPC('R');
with CurrentHedgehog^.Gear^ do
    Message:= Message or gm_Right
end;

procedure chRight_m(var s: shortstring);
begin
if CheckNoTeamOrHH then exit;
if not CurrentTeam^.ExtDriven then SendIPC('r');
with CurrentHedgehog^.Gear^ do
     Message:= Message and not gm_Right
end;

procedure chUp_p(var s: shortstring);
begin
if CheckNoTeamOrHH then exit;
bShowFinger:= false;
if not CurrentTeam^.ExtDriven then SendIPC('U');
with CurrentHedgehog^.Gear^ do
    Message:= Message or gm_Up
end;

procedure chUp_m(var s: shortstring);
begin
if CheckNoTeamOrHH then exit;
if not CurrentTeam^.ExtDriven then SendIPC('u');
with CurrentHedgehog^.Gear^ do
     Message:= Message and not gm_Up
end;

procedure chDown_p(var s: shortstring);
begin
if CheckNoTeamOrHH then exit;
bShowFinger:= false;
if not CurrentTeam^.ExtDriven then SendIPC('D');
with CurrentHedgehog^.Gear^ do
    Message:= Message or gm_Down
end;

procedure chDown_m(var s: shortstring);
begin
if CheckNoTeamOrHH then exit;
if not CurrentTeam^.ExtDriven then SendIPC('d');
with CurrentHedgehog^.Gear^ do
     Message:= Message and not gm_Down
end;

procedure chLJump(var s: shortstring);
begin
if CheckNoTeamOrHH then exit;
bShowFinger:= false;
if not CurrentTeam^.ExtDriven then SendIPC('j');
with CurrentHedgehog^.Gear^ do
    Message:= Message or gm_LJump
end;

procedure chHJump(var s: shortstring);
begin
if CheckNoTeamOrHH then exit;
bShowFinger:= false;
if not CurrentTeam^.ExtDriven then SendIPC('J');
with CurrentHedgehog^.Gear^ do
    Message:= Message or gm_HJump
end;

procedure chAttack_p(var s: shortstring);
begin
if CheckNoTeamOrHH then exit;
bShowFinger:= false;
with CurrentHedgehog^.Gear^ do
     begin
     {$IFDEF DEBUGFILE}AddFileLog('/+attack: Gear^.State = '+inttostr(State));{$ENDIF}
     if ((State and gstHHDriven) <> 0) then
        begin
        FollowGear:= CurrentHedgehog^.Gear;
        if not CurrentTeam^.ExtDriven then SendIPC('A');
        Message:= Message or gm_Attack
        end
     end
end;

procedure chAttack_m(var s: shortstring);
begin
if CheckNoTeamOrHH then exit;
with CurrentHedgehog^.Gear^ do
     begin
     if not CurrentTeam^.ExtDriven and
        ((Message and gm_Attack) <> 0) then SendIPC('a');
     Message:= Message and not gm_Attack
     end
end;

procedure chSwitch(var s: shortstring);
begin
if CheckNoTeamOrHH then exit;
if not CurrentTeam^.ExtDriven then SendIPC('S');
with CurrentHedgehog^.Gear^ do
     Message:= Message or gm_Switch
end;

procedure chNextTurn(var s: shortstring);
begin
if AllInactive then
   begin
   if not CurrentTeam^.ExtDriven then SendIPC('N');
   TickTrigger(trigTurns);
   {$IFDEF DEBUGFILE}AddFileLog('Doing SwitchHedgehog: time '+inttostr(GameTicks));{$ENDIF}
   end
end;

procedure chSay(var s: shortstring);
begin
SendIPC('s' + s);

if copy(s, 1, 4) = '/me ' then
	s:= '* ' + UserNick + ' ' + copy(s, 5, Length(s) - 4)
else
	s:= UserNick + ': ' + s;

AddChatString(s)
end;

procedure chTimer(var s: shortstring);
begin
if (s[0] <> #1) or (s[1] < '1') or (s[1] > '5') or (CurrentTeam = nil) then exit;
bShowFinger:= false;
if not CurrentTeam^.ExtDriven then SendIPC(s);
with CurrentHedgehog^.Gear^ do
     begin
     Message:= Message or gm_Timer;
     MsgParam:= byte(s[1]) - ord('0')
     end
end;

procedure chSlot(var s: shortstring);
var slot: LongWord;
begin
if (s[0] <> #1) or CheckNoTeamOrHH then exit;
bShowFinger:= false;
slot:= byte(s[1]) - 49;
if slot > cMaxSlotIndex then exit;
if not CurrentTeam^.ExtDriven then SendIPC(char(byte(s[1]) + 79));
with CurrentHedgehog^.Gear^ do
     begin
     Message:= Message or gm_Slot;
     MsgParam:= slot
     end
end;

procedure chSetWeapon(var s: shortstring);
begin
if (s[0] <> #1) or CheckNoTeamOrHH then exit;

if TAmmoType(s[1]) > High(TAmmoType) then exit;

if not CurrentTeam^.ExtDriven then SendIPC('w' + s);

with CurrentHedgehog^.Gear^ do
     begin
     Message:= Message or gm_Weapon;
     MsgParam:= byte(s[1])
     end
end;

procedure chTaunt(var s: shortstring);
begin
if (s[0] <> #1) or CheckNoTeamOrHH then exit;

if TWave(s[1]) > High(TWave) then exit;

if not CurrentTeam^.ExtDriven then SendIPC('t' + s);

with CurrentHedgehog^.Gear^ do
     begin
     Message:= Message or gm_Animate;
     MsgParam:= byte(s[1])
     end
end;

procedure doPut(putX, putY: LongInt; fromAI: boolean);
begin
if CheckNoTeamOrHH then exit;
if bShowAmmoMenu then
   begin
   bSelected:= true;
   exit
   end;

with CurrentHedgehog^.Gear^,
     CurrentHedgehog^ do
     if (State and gstHHChooseTarget) <> 0 then
        begin
        isCursorVisible:= false;
        if not CurrentTeam^.ExtDriven then
           begin
           if fromAI then
              begin
              TargetPoint.X:= putX;
              TargetPoint.Y:= putY
              end else
              begin
              SDL_GetMouseState(@TargetPoint.X, @TargetPoint.Y);
              dec(TargetPoint.X, WorldDx);
              dec(TargetPoint.Y, WorldDy)
              end;
           SendIPCXY('p', TargetPoint.X, TargetPoint.Y);
           end;
        State:= State and not gstHHChooseTarget;
        if (Ammo^[CurSlot, CurAmmo].Propz and ammoprop_AttackingPut) <> 0 then
           Message:= Message or gm_Attack;
        end else if CurrentTeam^.ExtDriven then OutError('got /put while not being in choose target mode', false)
end;

procedure chPut(var s: shortstring);
begin
doPut(0, 0, false)
end;

procedure chCapture(var s: shortstring);
begin
flagMakeCapture:= true
end;

procedure chSkip(var s: shortstring);
begin
if not CurrentTeam^.ExtDriven then SendIPC(',');
uStats.Skipped;
skipFlag:= true
end;

procedure chSetMap(var s: shortstring);
begin
if isDeveloperMode then
   begin
   Pathz[ptMapCurrent]:= Pathz[ptMaps] + '/' + s;
   InitStepsFlags:= InitStepsFlags or cifMap
   end
end;

procedure chSetTheme(var s: shortstring);
begin
if isDeveloperMode then
   begin
   Pathz[ptCurrTheme]:= Pathz[ptThemes] + '/' + s;
   InitStepsFlags:= InitStepsFlags or cifTheme
   end
end;

procedure chSetSeed(var s: shortstring);
begin
if isDeveloperMode then
   begin
   SetRandomSeed(s);
   cSeed:= s;
   InitStepsFlags:= InitStepsFlags or cifRandomize
   end
end;

procedure chAmmoMenu(var s: shortstring);
begin
if CheckNoTeamOrHH then exit;
with CurrentTeam^ do
     with Hedgehogs[CurrHedgehog] do
          begin
          bSelected:= false;

          if bShowAmmoMenu then bShowAmmoMenu:= false
          else if ((Gear^.State and (gstAttacking or gstAttacked)) <> 0) or (AttacksNum > 0)
               or ((Gear^.State and gstHHDriven) = 0) then else bShowAmmoMenu:= true
          end
end;

procedure chFullScr(var s: shortstring);
var flags: Longword;
{$IFDEF DEBUGFILE}
    buf: array[byte] of char;
{$ENDIF}
begin
if Length(s) = 0 then cFullScreen:= not cFullScreen
                 else cFullScreen:= s = '1';

{$IFDEF DEBUGFILE}
AddFileLog('Prepare to change video parameters...');
{$ENDIF}
SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

flags:= SDL_OPENGL;// or SDL_RESIZABLE;
if cFullScreen then
	begin
	flags:= flags or SDL_FULLSCREEN;
	cScreenWidth:= cInitWidth;
	cScreenHeight:= cInitHeight
	end
	else SDL_WM_SetCaption('Hedgewars', nil);
{$IFDEF DEBUGFILE}
AddFileLog('Freeing old primary surface...');
{$ENDIF}
SDL_FreeSurface(SDLPrimSurface);

SDLPrimSurface:= SDL_SetVideoMode(cScreenWidth, cScreenHeight, cBits, flags);
SDLTry(SDLPrimSurface <> nil, true);

{$IFDEF DEBUGFILE}
AddFileLog('Setting up OpenGL...');
{$ENDIF}
SetupOpenGL();

{$IFDEF DEBUGFILE}
AddFileLog('SDL video driver: ' + string(SDL_VideoDriverName(buf, sizeof(buf))));
{$ENDIF}
PixelFormat:= SDLPrimSurface^.format
end;

procedure chVol_p(var s: shortstring);
begin
inc(cVolumeDelta, 3)
end;

procedure chVol_m(var s: shortstring);
begin
dec(cVolumeDelta, 3)
end;

procedure chFindhh(var s: shortstring);
begin
if CheckNoTeamOrHH then exit;
bShowFinger:= true;
FollowGear:= CurrentHedgehog^.Gear
end;

procedure chPause(var s: shortstring);
begin
isPaused:= not isPaused;
SDL_ShowCursor(ord(isPaused))
end;

procedure chRotateMask(var s: shortstring);
const map: array[0..7] of byte = (7,4,0,1,2,3,0,5);
begin
cTagsMask:= map[cTagsMask]
end;

procedure chAddTrigger(var s: shortstring);
const MAXPARAMS = 16;
var params: array[0..Pred(MAXPARAMS)] of Longword;
    i: LongInt;
    c: char;
    tmp: shortstring;
begin
c:= s[1];
Delete(s, 1, 1);

i:= 0;
while (i < MAXPARAMS) and
      (Length(s) > 0) do
    begin
    SplitBySpace(s, tmp);
    val(s, params[i]);
    s:= tmp;
    inc(i)
    end;

case c of
  's': begin // sTYPE TICKS LIVES GEARTYPE X Y GEARTRIGGER
       TryDo(i = 7, errmsgWrongNumber, true);
       AddTriggerSpawner(params[0], params[1], params[2], TGearType(params[3]), params[4], params[5], params[6]);
       end;
  'C': begin
       TryDo(i = 3, errmsgWrongNumber, true);
       AddTriggerSuccess(params[0], params[1], params[2]);
       end;
  'F': begin
       TryDo(i = 3, errmsgWrongNumber, true);
       AddTriggerFail(params[0], params[1], params[2]);
       end;
  end
end;

procedure chSpeedup_p(var s: shortstring);
begin
isSpeed:= true
end;

procedure chSpeedup_m(var s: shortstring);
begin
isSpeed:= false
end;

procedure chChat(var s: shortstring);
begin
GameState:= gsChat;
KeyPressChat(27)
end;

procedure chHistory(var s: shortstring);
begin
uChat.showAll:= not uChat.showAll
end;