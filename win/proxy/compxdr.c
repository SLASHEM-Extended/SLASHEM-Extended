/* $Id: compxdr.c,v 1.4 2002-06-22 15:36:52 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2001-2002 */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "nhxdr.h"
#include "winproxy.h"
#include "proxycb.h"

/*
 * This module implements the XDR routines for non-trivial compound types that
 * NhExt needs.
 */

boolean proxy_xdr_init_nhwindow_req(xdr, datum)
NhExtXdr *xdr;
struct proxy_init_nhwindow_req *datum;
{
    return nhext_xdr_array(xdr, (char **)&datum->argv, 
      &datum->argc, (unsigned int)-1, sizeof(char *), nhext_xdr_wrapstring);
}

boolean proxy_xdr_init_nhwindow_res(xdr, datum)
NhExtXdr *xdr;
struct proxy_init_nhwindow_res *datum;
{
    int retval = nhext_xdr_bool(xdr, &datum->inited);
    retval &= nhext_xdr_array(xdr, (char **)&datum->argv, 
      &datum->argc, (unsigned int)-1, sizeof(char *), nhext_xdr_wrapstring);
    return retval;
}

boolean proxy_xdr_proxy_mi(xdrs, datum)
NhExtXdr *xdrs;
struct proxy_mi *datum;
{
    return nhext_xdr_int(xdrs, &datum->item) && nhext_xdr_long(xdrs, &datum->count);
}

boolean proxy_xdr_select_menu_res(xdr, datum)
NhExtXdr *xdr;
struct proxy_select_menu_res *datum;
{
    int retval;
    retval = nhext_xdr_int(xdr, &datum->retval);
    retval &= nhext_xdr_array(xdr, (char **)&datum->selected, 
      &datum->n, 0xffffffff, sizeof(struct proxy_mi), proxy_xdr_proxy_mi);
    return retval;
}

boolean proxy_xdr_status_req(xdr, datum)
NhExtXdr *xdr;
struct proxy_status_req *datum;
{
    return nhext_xdr_int(xdr, &datum->reconfig) &
      nhext_xdr_array(xdr, (char **)&datum->values, 
      &datum->nv, (unsigned int)-1, sizeof(char *), nhext_xdr_wrapstring);
}

boolean proxycb_xdr_get_player_choices_res_role(xdr, datum)
NhExtXdr *xdr;
struct proxycb_get_player_choices_res_role *datum;
{
    return nhext_xdr_wrapstring(xdr, (char **)&datum->male) &
      nhext_xdr_wrapstring(xdr, (char **)&datum->female);
}

boolean proxycb_xdr_get_player_choices_res(xdr, datum)
NhExtXdr *xdr;
struct proxycb_get_player_choices_res *datum;
{
    return nhext_xdr_array(xdr, (char **)&datum->aligns, &datum->n_aligns,
        (unsigned int)-1, sizeof(char *), nhext_xdr_wrapstring) &
      nhext_xdr_array(xdr, (char **)&datum->genders, &datum->n_genders,
        (unsigned int)-1, sizeof(char *), nhext_xdr_wrapstring) &
      nhext_xdr_array(xdr, (char **)&datum->races, &datum->n_races,
        (unsigned int)-1, sizeof(char *), nhext_xdr_wrapstring) &
      nhext_xdr_array(xdr, (char **)&datum->roles, &datum->n_roles,
        (unsigned int)-1, sizeof(struct proxycb_get_player_choices_res_role),
	proxycb_xdr_get_player_choices_res_role);
}
