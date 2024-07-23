import { Component, OnInit, Input, Output, EventEmitter } from '@angular/core';
import { ActivatedRoute, Router } from '@angular/router';
import { MatDialog, MatDialogRef } from '@angular/material/dialog';
import { Observable, Subject } from 'rxjs';
import { catchError, map } from 'rxjs/operators';
import { HttpEventType, HttpErrorResponse } from '@angular/common/http';
import { of } from 'rxjs';
import { BreakpointObserver, Breakpoints } from '@angular/cdk/layout';

import { Me }  from '../me';
import { Item }  from '../item';
import { Stream }  from '../stream';
import { ConversationComponent }  from '../conversation/conversation.component';

@Component({
  selector: 'app-idea',
  templateUrl: './idea.component.html',
  styleUrls: ['./idea.component.css']
})
export class IdeaComponent {

  @Input('me') me: Me;
  @Input('parent') parent: ConversationComponent;
  @Input('showSep') showSep: boolean;
  @Input('hideActionDates') hideActionDates: boolean;
  @Input('showBranch') showBranch: boolean;
  @Input('item') item: Item;
  @Input('submit') submit: boolean;
  @Input('width') width: string;
  @Input('returnblank') returnblank: boolean;
  @Input('hidemenu') hidemenu: boolean;
  @Input('hideName') hideName: boolean;
  @Input('hideText') hideText: boolean;
  @Input('stream') stream: Stream;
  @Input('noBackground') noBackground: boolean;
  @Input('collapsible') collapsible: boolean;
  @Input('modal') modal: boolean;
  @Input('noPadTop') noPadTop: boolean;

}
