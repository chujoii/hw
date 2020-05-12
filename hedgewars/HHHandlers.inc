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

////////////////////////////////////////////////////////////////////////////////
procedure ChangeAmmo(Gear: PGear);
var slot, i: Longword;
begin
slot:= Gear^.MsgParam;

with PHedgehog(Gear^.Hedgehog)^ do
	begin
	if ((Gear^.State and (gstAttacking or gstAttacked)) <> 0)
		or (AttacksNum > 0)
		or ((Gear^.State and gstHHDriven) = 0) then exit;

	Gear^.Message:= Gear^.Message and not (gm_LJump or gm_HJump or gm_Slot);

	if CurSlot = slot then
		begin
		i:= 0;
		repeat
		inc(CurAmmo);
		if (CurAmmo > cMaxSlotAmmoIndex) then
			begin
			CurAmmo:= 0;
			inc(i);
			TryDo(i < 2, 'Engine bug: no ammo in current slot', true)
			end;
		until (Ammo^[slot, CurAmmo].Count > 0) and (Team^.Clan^.TurnNumber > Ammoz[Ammo^[slot, CurAmmo].AmmoType].SkipTurns)
		end else
		begin
		i:= 0;
		// check whether there's ammo in slot
		while (i <= cMaxSlotAmmoIndex)
		  and ((Ammo^[slot, i].Count = 0)
		       or (Team^.Clan^.TurnNumber <= Ammoz[Ammo^[slot, i].AmmoType].SkipTurns)) do inc(i);
		
		if i <= cMaxSlotAmmoIndex then
			begin
			CurSlot:= slot;
			CurAmmo:= i
			end
		end
	end;

ApplyAmmoChanges(PHedgehog(Gear^.Hedgehog)^)
end;

procedure HHSetWeapon(Gear: PGear);
var t: LongInt;
    weap: TAmmoType;
begin
weap:= TAmmoType(Gear^.MsgParam);

if PHedgehog(Gear^.Hedgehog)^.Team^.Clan^.TurnNumber <= Ammoz[weap].SkipTurns then exit; // weapon isn't activated yet

Gear^.MsgParam:= Ammoz[weap].Slot;

t:= cMaxSlotAmmoIndex;

Gear^.Message:= Gear^.Message and not gm_Weapon;

with PHedgehog(Gear^.Hedgehog)^ do
	while (Ammo^[CurSlot, CurAmmo].AmmoType <> weap) and (t >= 0) do
		begin
		ChangeAmmo(Gear);
		dec(t)
		end
end;

procedure HHSetTimer(Gear: PGear);
begin
Gear^.Message:= Gear^.Message and not gm_Timer;
with PHedgehog(Gear^.Hedgehog)^ do
	if (Ammo^[CurSlot, CurAmmo].Propz and ammoprop_Timerable) <> 0 then
		begin
		Ammo^[CurSlot, CurAmmo].Timer:= 1000 * Gear^.MsgParam;
		with CurrentTeam^ do
			ApplyAmmoChanges(Hedgehogs[CurrHedgehog]);
		end;
end;


procedure Attack(Gear: PGear);
var xx, yy: hwFloat;
begin
with Gear^,
     PHedgehog(Gear^.Hedgehog)^ do
     begin
     if ((State and gstHHDriven) <> 0)and
        ((State and (gstAttacked or gstHHChooseTarget)) = 0)and
        (((State and gstMoving) = 0) or ((Ammo^[CurSlot, CurAmmo].Propz and ammoprop_AttackInMove) <> 0))and
        ((TargetPoint.X <> NoPointX) or ((Ammo^[CurSlot, CurAmmo].Propz and ammoprop_NeedTarget) = 0)) then
        begin
        State:= State or gstAttacking;
        if Power = cMaxPower then Message:= Message and not gm_Attack
        else if (Ammo^[CurSlot, CurAmmo].Propz and ammoprop_Power) = 0 then Message:= Message and not gm_Attack
        else begin
             if Power = 0 then
                begin
                AttackBar:= CurrentTeam^.AttackBar;
                PlaySound(sndThrowPowerUp, false)
                end;
             inc(Power)
             end;
        if ((Message and gm_Attack) <> 0) then exit;

        if (Ammo^[CurSlot, CurAmmo].Propz and ammoprop_Power) <> 0 then
           begin
           StopSound(sndThrowPowerUp);
           PlaySound(sndThrowRelease, false);
           end;
        xx:= SignAs(AngleSin(Angle), dX);
        yy:= -AngleCos(Angle);

        if ((Gear^.State and gstHHHJump) <> 0) then xx:= - xx;
             case Ammo^[CurSlot, CurAmmo].AmmoType of
                      amGrenade: FollowGear:= AddGear(hwRound(X), hwRound(Y), gtAmmo_Bomb,    0, xx*Power/cPowerDivisor, yy*Power/cPowerDivisor, Ammo^[CurSlot, CurAmmo].Timer);
                  amClusterBomb: FollowGear:= AddGear(hwRound(X), hwRound(Y), gtClusterBomb,  0, xx*Power/cPowerDivisor, yy*Power/cPowerDivisor, Ammo^[CurSlot, CurAmmo].Timer);
                      amBazooka: FollowGear:= AddGear(hwRound(X), hwRound(Y), gtAmmo_Grenade, 0, xx*Power/cPowerDivisor, yy*Power/cPowerDivisor, 0);
                          amUFO: FollowGear:= AddGear(hwRound(X), hwRound(Y), gtUFO,          0, xx*Power/cPowerDivisor, yy*Power/cPowerDivisor, 0);
                      amShotgun: begin
                                 PlaySound(sndShotgunReload, false);
                                 CurAmmoGear:= AddGear(hwRound(X), hwRound(Y), gtShotgunShot,  0, xx * _0_5, yy * _0_5, 0);
                                 end;
                   amPickHammer: CurAmmoGear:= AddGear(hwRound(Gear^.X), hwRound(Gear^.Y) + cHHRadius, gtPickHammer, 0, _0, _0, 0);
                         amSkip: ParseCommand('/skip', true);
                         amRope: CurAmmoGear:= AddGear(hwRound(Gear^.X), hwRound(Gear^.Y), gtRope, 0, xx, yy, 0);
                         amMine: begin
                                 AddGear(hwRound(X) + hwSign(dX) * 7, hwRound(Y), gtMine, 0, SignAs(_0_02, dX), _0, 3000);
                                 PlaySound(sndLaugh, false)
                                 end;
                       amDEagle: CurAmmoGear:= AddGear(hwRound(X + xx * cHHRadius), hwRound(Y + yy * cHHRadius), gtDEagleShot, 0, xx * _0_5, yy * _0_5, 0);
                     amDynamite: begin
                                 AddGear(hwRound(X) + hwSign(dX) * 7, hwRound(Y), gtDynamite, 0, SignAs(_0_03, dX), _0, 5000);
                                 PlaySound(sndLaugh, false)
                                 end;
                    amFirePunch: CurAmmoGear:= AddGear(hwRound(X) + hwSign(dX) * 10, hwRound(Y), gtFirePunch, 0, xx, _0, 0);
                         amWhip: CurAmmoGear:= AddGear(hwRound(X) + hwSign(dX) * 10, hwRound(Y), gtWhip, 0, SignAs(_1, dX), - _0_8, 0);
                  amBaseballBat: CurAmmoGear:= AddGear(hwRound(X) + hwSign(dX) * 10, hwRound(Y), gtShover, 0, xx * _0_5, yy * _0_5, 0);
                    amParachute: CurAmmoGear:= AddGear(hwRound(X), hwRound(Y), gtParachute, 0, _0, _0, 0);
                    amAirAttack: AddGear(Ammo^[CurSlot, CurAmmo].Pos, 0, gtAirAttack, 0, _0, _0, 0);
                   amMineStrike: AddGear(Ammo^[CurSlot, CurAmmo].Pos, 0, gtAirAttack, 1, _0, _0, 0);
                    amBlowTorch: CurAmmoGear:= AddGear(hwRound(X), hwRound(Y), gtBlowTorch, 0, SignAs(_0_5, dX), _0, 0);
                       amGirder: CurAmmoGear:= AddGear(0, 0, gtGirder, Ammo^[CurSlot, CurAmmo].Pos, _0, _0, 0);
                     amTeleport: CurAmmoGear:= AddGear(0, 0, gtTeleport, 0, _0, _0, 0);
                       amSwitch: CurAmmoGear:= AddGear(hwRound(X), hwRound(Y), gtSwitcher, 0, _0, _0, 0);
                       amMortar: FollowGear:= AddGear(hwRound(X), hwRound(Y), gtMortar,  0, xx*cMaxPower/cPowerDivisor, yy*cMaxPower/cPowerDivisor, 0);
                       amKamikaze: CurAmmoGear:= AddGear(hwRound(X), hwRound(Y), gtKamikaze, 0, xx * _0_5, yy * _0_5, 0);
                         amCake: begin
                                 CurAmmoGear:= AddGear(hwRound(X) + hwSign(dX) * 3, hwRound(Y), gtCake, 0, xx, _0, 0);
                                 PlaySound(sndLaugh, false)
                                 end;
                    amSeduction: CurAmmoGear:= AddGear(hwRound(X + xx * cHHRadius * 2), hwRound(Y + yy * cHHRadius * 2), gtSeduction, 0, xx * _0_4, yy * _0_4, 0);
                   amWatermelon: FollowGear:= AddGear(hwRound(X), hwRound(Y), gtWatermelon,  0, xx*Power/cPowerDivisor, yy*Power/cPowerDivisor, Ammo^[CurSlot, CurAmmo].Timer);
                  amHellishBomb: FollowGear:= AddGear(hwRound(X), hwRound(Y), gtHellishBomb,    0, xx*Power/cPowerDivisor, yy*Power/cPowerDivisor, 5000);
                  end;

        uStats.AmmoUsed(Ammo^[CurSlot, CurAmmo].AmmoType);

        Power:= 0;
        if (CurAmmoGear <> nil)
           and (((Ammo^[CurSlot, CurAmmo].Propz) and ammoprop_AltUse) = 0){check for dropping ammo from rope} then
           begin
           CurAmmoGear^.Ammo:= @(Ammo^[CurSlot, CurAmmo]);
           Message:= Message or gm_Attack;
           CurAmmoGear^.Message:= Message
           end else begin
           if not CurrentTeam^.ExtDriven and
             ((Ammo^[CurSlot, CurAmmo].Propz and ammoprop_Power) <> 0) then SendIPC('a');
           AfterAttack
           end
        end else Message:= Message and not gm_Attack
     end
end;

procedure AfterAttack;
begin
with CurrentHedgehog^.Gear^,
		CurrentHedgehog^ do
	begin
		Inc(AttacksNum);
		State:= State and not gstAttacking;
		if (Ammo^[CurSlot, CurAmmo].NumPerTurn >= AttacksNum) or
		((GameFlags and gfMultiWeapon) <> 0) then isInMultiShoot:= true
		else begin
		TurnTimeLeft:= Ammoz[Ammo^[CurSlot, CurAmmo].AmmoType].TimeAfterTurn;
		State:= State or gstAttacked;
		OnUsedAmmo(CurrentHedgehog^)
		end;
	AttackBar:= 0;
	end
end;

////////////////////////////////////////////////////////////////////////////////
procedure doStepHedgehogDead(Gear: PGear);
const frametime = 200;
      timertime = frametime * 6;
begin
if Gear^.Timer > 1 then
	begin
	AllInactive:= false;
	dec(Gear^.Timer);
	if (Gear^.Timer mod frametime) = 0 then inc(Gear^.Pos)
	end else
if Gear^.Timer = 1 then
	begin
	Gear^.State:= Gear^.State or gstNoDamage;
	doMakeExplosion(hwRound(Gear^.X), hwRound(Gear^.Y), 30, EXPLAutoSound);
	AddGear(hwRound(Gear^.X), hwRound(Gear^.Y), gtGrave, 0, _0, _0, 0)^.Hedgehog:= Gear^.Hedgehog;
	DeleteGear(Gear);
	SetAllToActive
	end else // Gear^.Timer = 0
	begin
	AllInactive:= false;
	Gear^.Z:= cCurrHHZ;
	RemoveGearFromList(Gear);
	InsertGearToList(Gear);
	PlaySound(sndByeBye, false);
	Gear^.Pos:= 0;
	Gear^.Timer:= timertime
	end
end;

////////////////////////////////////////////////////////////////////////////////
procedure PickUp(HH, Gear: PGear);
var s: shortstring;
    a: TAmmoType;
begin
Gear^.Message:= gm_Destroy;
case Gear^.Pos of
       posCaseAmmo: begin
                    a:= TAmmoType(Gear^.State);
                    AddAmmo(PHedgehog(HH^.Hedgehog)^, a);
                    if (not (PHedgehog(HH^.Hedgehog)^.Team^.ExtDriven
                      or (PHedgehog(HH^.Hedgehog)^.BotLevel > 0)))
                    or (GameType = gmtDemo)  then
                       begin
                       s:= trammo[Ammoz[a].NameId] + '(+' + IntToStr(Ammoz[a].NumberInCase) + ')';
                       AddCaption(s, PHedgehog(HH^.Hedgehog)^.Team^.Clan^.Color, capgrpAmmoinfo);
                       end
                    end;
     posCaseHealth: begin
                    inc(HH^.Health, Gear^.Health);
                    str(Gear^.Health, s);
                    s:= '+' + s;
                    AddCaption(s, PHedgehog(HH^.Hedgehog)^.Team^.Clan^.Color, capgrpAmmoinfo);
                    RenderHealth(PHedgehog(HH^.Hedgehog)^);
                    RecountTeamHealth(PHedgehog(HH^.Hedgehog)^.Team)
                    end;
     end
end;

const StepTicks: LongWord = 0;

procedure HedgehogStep(Gear: PGear);
var PrevdX: LongInt;
begin
if ((Gear^.State and (gstAttacking or gstMoving)) = 0) then
   begin
   if isCursorVisible then
      with PHedgehog(Gear^.Hedgehog)^ do
        with Ammo^[CurSlot, CurAmmo] do
          begin
          if (Gear^.Message and gm_Left  ) <> 0 then
             Pos:= (Pos + Ammoz[AmmoType].PosCount - 1) mod Ammoz[AmmoType].PosCount
          else
          if (Gear^.Message and gm_Right ) <> 0 then
             Pos:= (Pos + 1) mod Ammoz[AmmoType].PosCount
          else exit;
          StepTicks:= 200;
          exit
          end;

	if ((Gear^.Message and gm_Animate) <> 0) then
		begin
		Gear^.Message:= 0;
		Gear^.State:= Gear^.State or gstAnimation;
		Gear^.Tag:= Gear^.MsgParam;
		Gear^.Timer:= 0;
		Gear^.Pos:= 0
		end;

   if ((Gear^.Message and gm_LJump ) <> 0) then
      begin
      Gear^.Message:= Gear^.Message and not gm_LJump;
      DeleteCI(Gear);
      if not TestCollisionYwithGear(Gear, -1) then
         if not TestCollisionXwithXYShift(Gear, _0, -2, hwSign(Gear^.dX)) then Gear^.Y:= Gear^.Y - _2 else
         if not TestCollisionXwithXYShift(Gear, _0, -1, hwSign(Gear^.dX)) then Gear^.Y:= Gear^.Y - _1;
      if not (TestCollisionXwithGear(Gear, hwSign(Gear^.dX))
         or   TestCollisionYwithGear(Gear, -1)) then
         begin
         Gear^.dY:= -_0_15;
         Gear^.dX:= SignAs(_0_15, Gear^.dX);
         Gear^.State:= Gear^.State or gstMoving or gstHHJumping;
         PlaySound(sndJump1, false);
         exit
         end;
      end;

   if ((Gear^.Message and gm_HJump ) <> 0) then
      begin
      DeleteCI(Gear);
      Gear^.Message:= Gear^.Message and not gm_HJump;

      Gear^.dY:= -_0_2;
      SetLittle(Gear^.dX);
      Gear^.State:= Gear^.State or gstMoving or gstHHJumping;
      PlaySound(sndJump3, false);
      exit
      end;

   PrevdX:= hwSign(Gear^.dX);
   if (Gear^.Message and gm_Left  )<>0 then Gear^.dX:= -cLittle else
   if (Gear^.Message and gm_Right )<>0 then Gear^.dX:=  cLittle else exit;

   StepTicks:= cHHStepTicks;
   if PrevdX <> hwSign(Gear^.dX) then
      begin
      FollowGear:= Gear;
      exit
      end;
   DeleteCI(Gear); // must be after exit!! (see previous line)

   PHedgehog(Gear^.Hedgehog)^.visStepPos:= (PHedgehog(Gear^.Hedgehog)^.visStepPos + 1) and 7;
   if TestCollisionXwithGear(Gear, hwSign(Gear^.dX)) then
      begin
      if not (TestCollisionXwithXYShift(Gear, _0, -6, hwSign(Gear^.dX))
         or TestCollisionYwithGear(Gear, -1)) then Gear^.Y:= Gear^.Y - _1;
      if not (TestCollisionXwithXYShift(Gear, _0, -5, hwSign(Gear^.dX))
         or TestCollisionYwithGear(Gear, -1)) then Gear^.Y:= Gear^.Y - _1;
      if not (TestCollisionXwithXYShift(Gear, _0, -4, hwSign(Gear^.dX))
         or TestCollisionYwithGear(Gear, -1)) then Gear^.Y:= Gear^.Y - _1;
      if not (TestCollisionXwithXYShift(Gear, _0, -3, hwSign(Gear^.dX))
         or TestCollisionYwithGear(Gear, -1)) then Gear^.Y:= Gear^.Y - _1;
      if not (TestCollisionXwithXYShift(Gear, _0, -2, hwSign(Gear^.dX))
         or TestCollisionYwithGear(Gear, -1)) then Gear^.Y:= Gear^.Y - _1;
      if not (TestCollisionXwithXYShift(Gear, _0, -1, hwSign(Gear^.dX))
         or TestCollisionYwithGear(Gear, -1)) then Gear^.Y:= Gear^.Y - _1;
      end;
   if not TestCollisionXwithGear(Gear, hwSign(Gear^.dX)) then Gear^.X:= Gear^.X + SignAs(_1, Gear^.dX);

   SetAllHHToActive;

   if not TestCollisionYwithGear(Gear, 1) then
   begin
   Gear^.Y:= Gear^.Y + _1;
   if not TestCollisionYwithGear(Gear, 1) then
   begin
   Gear^.Y:= Gear^.Y + _1;
   if not TestCollisionYwithGear(Gear, 1) then
   begin
   Gear^.Y:= Gear^.Y + _1;
   if not TestCollisionYwithGear(Gear, 1) then
   begin
   Gear^.Y:= Gear^.Y + _1;
   if not TestCollisionYwithGear(Gear, 1) then
   begin
   Gear^.Y:= Gear^.Y + _1;
   if not TestCollisionYwithGear(Gear, 1) then
   begin
   Gear^.Y:= Gear^.Y + _1;
   if not TestCollisionYwithGear(Gear, 1) then
      begin
      Gear^.Y:= Gear^.Y - _6;
      Gear^.dY:= _0;
      Gear^.State:= Gear^.State or gstMoving;
      exit
      end;
   end
   end
   end
   end
   end
   end;
   AddGearCI(Gear)
   end
end;

procedure HedgehogChAngle(Gear: PGear);
var da: LongWord;
begin
with PHedgehog(Gear^.Hedgehog)^ do
	if (Ammo^[CurSlot, CurAmmo].AmmoType = amRope)
	and ((Gear^.State and (gstMoving or gstHHJumping)) = gstMoving) then da:= 2 else da:= 1;

if ((Gear^.Message and gm_Up) <> 0) and (Gear^.Angle - da >= CurMinAngle) then dec(Gear^.Angle, da)
else
if ((Gear^.Message and gm_Down) <> 0) and (Gear^.Angle + da <= CurMaxAngle) then inc(Gear^.Angle, da);
end;

procedure doStepHedgehog(Gear: PGear); forward;
////////////////////////////////////////////////////////////////////////////////
procedure doStepHedgehogMoving(Gear: PGear);
var isFalling: boolean;
begin
isFalling:= (Gear^.dY.isNegative) or not TestCollisionYKick(Gear, 1);
if isFalling then
   begin
   if (Gear^.dY.isNegative) and TestCollisionYKick(Gear, -1) then Gear^.dY:= _0;
   Gear^.State:= Gear^.State or gstMoving;
   Gear^.dY:= Gear^.dY + cGravity
   end else
   begin
   if ((hwAbs(Gear^.dX) + hwAbs(Gear^.dY)) < _0_55)
      and ((Gear^.State and gstHHJumping) <> 0) then SetLittle(Gear^.dX);

   if not Gear^.dY.isNegative then
      begin
      CheckHHDamage(Gear);

      if ((Gear^.State and gstHHHJump) <> 0) and
         (Gear^.dX.QWordValue < _0_02.QWordValue) then Gear^.dX.isNegative:= not Gear^.dX.isNegative; // landing after high jump

      Gear^.State:= Gear^.State and not (gstHHJumping or gstHHHJump);
      Gear^.dY:= _0;
      end else Gear^.dY:= Gear^.dY + cGravity;

   if ((Gear^.State and gstMoving) <> 0) then Gear^.dX:= Gear^.dX * Gear^.Friction
   end;

if (Gear^.State <> 0) then DeleteCI(Gear);

if (Gear^.State and gstMoving) <> 0 then
   if TestCollisionXKick(Gear, hwSign(Gear^.dX)) then
      if not isFalling then
         if hwAbs(Gear^.dX) > _0_01 then
            if not TestCollisionXwithXYShift(Gear, int2hwFloat(hwSign(Gear^.dX)) - Gear^.dX, -1, hwSign(Gear^.dX)) then begin Gear^.X:= Gear^.X + Gear^.dX; Gear^.dX:= Gear^.dX * _0_96; Gear^.Y:= Gear^.Y - _1 end else
            if not TestCollisionXwithXYShift(Gear, int2hwFloat(hwSign(Gear^.dX)) - Gear^.dX, -2, hwSign(Gear^.dX)) then begin Gear^.X:= Gear^.X + Gear^.dX; Gear^.dX:= Gear^.dX * _0_93; Gear^.Y:= Gear^.Y - _2 end else
            if not TestCollisionXwithXYShift(Gear, int2hwFloat(hwSign(Gear^.dX)) - Gear^.dX, -3, hwSign(Gear^.dX)) then begin Gear^.X:= Gear^.X + Gear^.dX; Gear^.dX:= Gear^.dX * _0_9 ; Gear^.Y:= Gear^.Y - _3 end else
            if not TestCollisionXwithXYShift(Gear, int2hwFloat(hwSign(Gear^.dX)) - Gear^.dX, -4, hwSign(Gear^.dX)) then begin Gear^.X:= Gear^.X + Gear^.dX; Gear^.dX:= Gear^.dX * _0_87; Gear^.Y:= Gear^.Y - _4 end else
            if not TestCollisionXwithXYShift(Gear, int2hwFloat(hwSign(Gear^.dX)) - Gear^.dX, -5, hwSign(Gear^.dX)) then begin Gear^.X:= Gear^.X + Gear^.dX; Gear^.dX:= Gear^.dX * _0_84; Gear^.Y:= Gear^.Y - _5 end else
            if hwAbs(Gear^.dX) > _0_02 then Gear^.dX:= -Gear^.Elasticity * Gear^.dX
                                   else begin
                                        Gear^.State:= Gear^.State and not gstMoving;
                                        SetLittle(Gear^.dX)
                                        end
            else begin
                 Gear^.State:= Gear^.State and not gstMoving;
                 SetLittle(Gear^.dX)
                 end
         else if hwAbs(Gear^.dX) > cLittle then Gear^.dX:= -Gear^.Elasticity * Gear^.dX
                                           else SetLittle(Gear^.dX);

if (not isFalling) and
   (hwAbs(Gear^.dX) + hwAbs(Gear^.dY) < _0_03) then
   begin
   Gear^.State:= Gear^.State and not gstMoving;
   SetLittle(Gear^.dX);
   Gear^.dY:= _0
   end else Gear^.State:= Gear^.State or gstMoving;

if (Gear^.State and gstMoving) <> 0 then
   begin
   Gear^.State:= Gear^.State and not gstAnimation;
   Gear^.X:= Gear^.X + Gear^.dX;
   Gear^.Y:= Gear^.Y + Gear^.dY;
   if (not Gear^.dY.isNegative) and
      (not TestCollisionYKick(Gear, 1)) and
       TestCollisionYwithXYShift(Gear, 0, 1, 1) then
      begin
      CheckHHDamage(Gear);
      Gear^.dY:= _0;
      Gear^.Y:= Gear^.Y + _1
      end;
   CheckGearDrowning(Gear)
   end
end;

procedure doStepHedgehogDriven(Gear: PGear);
var t: PGear;
begin
if not isInMultiShoot then
   AllInactive:= false
else
   Gear^.Message:= 0;

if (TurnTimeLeft = 0) or (Gear^.Damage > 0) then
   begin
   TurnTimeLeft:= 0;
   Gear^.State:= Gear^.State and not (gstHHDriven or gstAnimation);
   if Gear^.Damage > 0 then
      Gear^.State:= Gear^.State and not (gstHHJumping or gstHHHJump);
   exit
   end;

if (Gear^.State and gstAnimation) <> 0 then
	begin
	Gear^.Message:= 0;
	inc(Gear^.Timer);
	if Gear^.Timer = 125 then
		begin
		Gear^.Timer:= 0;
		inc(Gear^.Pos);
		if Gear^.Pos = Wavez[TWave(Gear^.Tag)].FramesCount then
			Gear^.State:= Gear^.State and not gstAnimation
		end;
	exit
	end;

if ((Gear^.State and gstMoving) <> 0)
    or (StepTicks = cHHStepTicks)
	or (CurAmmoGear <> nil) then // we're moving
	begin
	with PHedgehog(Gear^.Hedgehog)^ do
		if (CurAmmoGear = nil)
		and (Gear^.dY > _0_39)
		and (Ammo^[CurSlot, CurAmmo].AmmoType = amParachute) then Gear^.Message:= Gear^.Message or gm_Attack;
	// check for case with ammo
	t:= CheckGearNear(Gear, gtCase, 36, 36);
	if t <> nil then
		PickUp(Gear, t)
	end;

if (CurAmmoGear = nil) then
	if (((Gear^.Message and gm_Attack) <> 0)
		or ((Gear^.State and gstAttacking) <> 0)) then
		Attack(Gear) // should be before others to avoid desync with '/put' msg and changing weapon msgs
	else
else with PHedgehog(Gear^.Hedgehog)^ do
	 if ((CurAmmoGear^.Ammo^.Propz and ammoprop_AltAttack) <> 0) 
		and ((Gear^.Message and gm_LJump) <> 0)
		and (((Ammo^[CurSlot, CurAmmo].Propz) and ammoprop_AltUse) <> 0) then
		begin
		Gear^.Message:= Gear^.Message and not gm_LJump;
		Attack(Gear)
		end;

if (CurAmmoGear = nil)
	or ((CurAmmoGear^.Ammo^.Propz and ammoprop_AltAttack) <> 0) then
	begin
	if ((Gear^.Message and gm_Slot) <> 0) then ChangeAmmo(Gear);

	if ((Gear^.Message and gm_Weapon) <> 0) then HHSetWeapon(Gear);

	if ((Gear^.Message and gm_Timer) <> 0) then HHSetTimer(Gear);
	end;

if CurAmmoGear <> nil then
   begin
   CurAmmoGear^.Message:= Gear^.Message;
   exit
   end;

if not isInMultiShoot then
   HedgehogChAngle(Gear);

if (Gear^.State and gstMoving) <> 0 then
   begin
   if ((Gear^.Message and gm_HJump) <> 0) and
      ((Gear^.State and gstHHJumping) <> 0) and
      ((Gear^.State and gstHHHJump) = 0) then
      if (not (hwAbs(Gear^.dX) > cLittle)) and (Gear^.dY < -_0_02) then
         begin
         Gear^.State:= Gear^.State or gstHHHJump or gstMoving;
         Gear^.dY:= -_0_25;
         Gear^.dX:= -SignAs(_0_02, Gear^.dX);
         PlaySound(sndJump2, false)
         end;
   Gear^.Message:= Gear^.Message and not (gm_LJump or gm_HJump);

   if ((Gear^.State and gstHHJumping) <> 0) and
      TestCollisionXwithGear(Gear, hwSign(Gear^.dX)) then SetLittle(Gear^.dX);

   doStepHedgehogMoving(Gear);

   if ((Gear^.State and (gstMoving or gstDrowning)) = 0) then
      begin
      AddGearCI(Gear);
      StepTicks:= 350
      end;
   exit
   end;

	if not isInMultiShoot then
		begin
		if StepTicks > 0 then dec(StepTicks);
		if (StepTicks = 0) then HedgehogStep(Gear)
		end
end;

////////////////////////////////////////////////////////////////////////////////
procedure doStepHedgehogFree(Gear: PGear);
var prevState: Longword;
begin
prevState:= Gear^.State;

doStepHedgehogMoving(Gear);

if (Gear^.State and (gstMoving or gstDrowning)) <> 0 then
	begin
	if Gear^.Damage > 0 then CalcRotationDirAngle(Gear);
	AllInactive:= false;
	exit
	end;

if (Gear^.Health = 0) then
	begin
	if PrvInactive then
		begin
		Gear^.Timer:= 0;
		Gear^.State:= Gear^.State or gstHHDeath;
		Gear^.doStep:= @doStepHedgehogDead;
		FollowGear:= Gear;
		PrvInactive:= false;
		AllInactive:= false
		end;
	exit
	end;

if ((Gear^.State and gstWait) = 0) and
	(prevState <> Gear^.State) then
	begin
	Gear^.State:= gstWait;
	Gear^.Timer:= 150
	end else
	begin
	if Gear^.Timer = 0 then
		begin
		Gear^.State:= 0;
		Gear^.Active:= false;
		AddGearCI(Gear);
		exit
		end else dec(Gear^.Timer)
	end;

AllInactive:= false
end;

////////////////////////////////////////////////////////////////////////////////
procedure doStepHedgehog(Gear: PGear);
begin
if (Gear^.Message and gm_Destroy) <> 0 then
   begin
   DeleteGear(Gear);
   exit
   end;
if (Gear^.State and gstHHDriven) = 0 then doStepHedgehogFree(Gear)
                                     else doStepHedgehogDriven(Gear)
end;