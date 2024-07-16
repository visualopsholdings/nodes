import { Component, OnInit, OnDestroy, Input, SimpleChange, ViewChild, ElementRef, EventEmitter } from '@angular/core';
import { Router, ActivatedRoute, NavigationEnd } from '@angular/router';
import { MatDialog } from '@angular/material/dialog';
import { interval, Observable } from 'rxjs';
import { debounce } from 'rxjs/operators';
import { BreakpointObserver, Breakpoints } from '@angular/cdk/layout';
import { HostListener } from '@angular/core';
import { FormControl } from '@angular/forms';

import * as _ from 'lodash';

import { Me }  from '../me';
import { Stream }  from '../stream';
import { User }  from '../user';

import { ConfirmComponent } from '../confirm/confirm.component';
import { AppComponent } from '../app.component';
import { IconService }  from '../icon.service';
import { StreamService }  from '../stream.service';
import { AddStreamDialogComponent } from '../add-stream-dialog/add-stream-dialog.component';
import { SocketService }  from '../socket.service';
import { StreamOpenService }  from '../stream-open.service';
import { BootstrapService } from '../bootstrap.service';
import { StreamFlags } from '../../../../shared-ui/streamflags';

@Component({
  selector: 'app-streams',
  templateUrl: './streams.component.html',
  styleUrls: ['./streams.component.css']
})
export class StreamsComponent implements OnInit, OnDestroy {

  @Input('main') main: AppComponent;

  me: Me;

  streams: Stream[];
  listheight = "160px";
  unread = 0;
  inFront = true;
  beep = new Audio("data:audio/mpeg;base64,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");
  opened = false;
  sockets = true;
  selectedStream: Stream;
  captive = false;

  private token: string;
  private viewOnly = false;
  private gridView = false;
  private anonymous = false;
  private onMsg = new EventEmitter<any>();
  private onUpdate = new EventEmitter<any>();
  private onStatus = new EventEmitter<any>();
  private streamWaiting = null;

  private minStreamsForMarket = 10;
  private maxStreams = 1000;

  @HostListener('window:focus', ['$event'])
  onFocus(event: FocusEvent): void {
    this.nowInFront();
  }

  @HostListener('window:blur', ['$event'])
  onBlur(event: FocusEvent): void {
     this.nowBehind();
  }

  @HostListener('document:visibilitychange', ['$event'])
  onVisibilityChange(event: any): void {
    if (document.visibilityState == "hidden") {
      this.nowBehind();
    }
    else if (document.visibilityState == "visible") {
      this.nowInFront();
    }
  }

  private nowBehind() {
    this.inFront = false;
  }

  private nowInFront() {
    if (!this.inFront) {
      this.inFront = true;
      this.getStreams();
    }
  }

  constructor(
    public dialog: MatDialog,
    private router: Router,
    private route: ActivatedRoute,
    private breakpointObserver: BreakpointObserver,
    private iconService: IconService,
    private streamService: StreamService,
    private showActionsService: ShowActionsService,
    private streamOpenService: StreamOpenService,
    private roomOpenService: RoomOpenService,
    private socketService: SocketService,
    private settingsService: SettingsService,
    private userListService: UserListService,
    private bootstrapService: BootstrapService,
    private mediarouterService: MediarouterService,
  ) {
/*    if (this.router.url.indexOf("/streams/") == 0) {
      var stream = this.router.url.substr(9);
      let q = stream.indexOf("?");
      if (q >= 0) {
        stream = stream.substr(0, q);
      }
      this.streamService.getStream(stream).subscribe(stream => {
        this.selectedStream = stream;
      });
    }*/
    route.queryParams.subscribe(params => {
      this.sockets = params['sockets'] != "false";
      this.captive = params['captive'] == "true";
      this.userListService.setSockets(this.sockets);
      let extid = params['extsocket'];
      if (extid) {
        this.socketService.setExtID(extid);
      }
      this.token = params['token'];
    });
    router.events.subscribe(change => {
      if (change instanceof NavigationEnd) {
        this.onResize(null);
      }
    });
  }

  ngOnInit() {
    this.setTitle();
    this.streamOpenService.openChanged$.subscribe(change => {
      if (change.refresh) {
        this.getStreams();
        if (change.clear) {
          this.selectedStream = null;
        }
        this.onResize(null);
      }
      else {
        if (change.streams && change.streams.length > 0) {
          this.streamService.getSelectedStreams(change.streams).subscribe(streams => {
            if (streams && streams.length > 0) {
              var newstreams = [];
              if (!this.streams) {
                this.streams = [];
              }
              this.streams.forEach(s => newstreams.push(s));
              streams.forEach(e => {
                if (this.streams.filter(e2 => e2._id == e._id).length == 0) {
                  newstreams.push(e);
                }
              });
              this.loadStreams(newstreams);
              var ids = newstreams.map(e => e._id);
              this.loadSockets(_.uniq(ids));
              this.selectedStream = streams[0];
              this.opened = this.shouldShowAtAll();
              this.showStream(this.selectedStream._id);
              this.onResize(null);
            }
          });
        }
        else {
          this.opened = change.open && this.shouldShowAtAll();
          if (this.opened) {
            var stream = change.streams && change.streams.length > 0 ? change.streams[0] : null;
            if (stream) {
              this.showStream(stream);
              this.onResize(null);
            }
            else {
              this.selectedStream = null;
              this.getStreams();
            }
          }
        }
      }
    });
    this.roomOpenService.openChanged$.subscribe(change => {
      this.streams.forEach(e => {
        if (e._id == change.stream) {
          e.room = change.open;
        }
      });
    });
    this.bootstrapService.meChanged$.subscribe(me => {
      [this.me, this.flags, this.site] = me;
      if (this.sockets) {
        this.onMsg.subscribe(message => {
    //      console.log("streams", "onMsg", message);
          if (this.inFront) {
            if (!this.selectedStream || message.stream != this.selectedStream._id) {
              this.getStreams();
            }
          }
          else {
            if (this.me.notifySounds) {
              this.beep.play();
            }
          }
        });
        this.onUpdate.subscribe(data => {
    //      console.log("streams", "onUpdate", data);
          if (data.type == "stream" && data.id == "streams") {
            if (data.newstream) {
              this.main.newstreams++;
            }
            this.getStreams();
          }
          else if (!this.selectedStream || (data.type == "stream" && data.id != this.selectedStream._id)) {
            if (data.idea) {
              this.updateCountOnStream(data.id);
            }
            else {
              this.getStreams();
            }
          }
        });
        this.onStatus.subscribe(status => {
          if (status.text.indexOf("Discovery complete") >= 0) {
            if (this.streamWaiting) {
              this.streamService.searchStreams(this.streamWaiting).subscribe(streams => {
                if (streams.length > 0) {
                  this.selectStream(streams[0]);
                }
              });
            }
          }
        });
        this.socketService.registerMsg(this.onMsg);
        this.socketService.registerUpdate(this.onUpdate);
        this.socketService.registerStatus(this.onStatus);
      }
      this.onResize(null);
      if (this.hasPublicRedirect()) {
        this.router.navigateByUrl(this.site.publicStreamURL);
      }
      this.streamOpenService.refresh(false);
    });
  }

  ngOnDestroy() {
  }

  private updateCountOnStream(stream: string) {
    this.streams.forEach(s => {
      if (s._id == stream) {
        s.unread++;
      }
    });
  }

  private showStream(id: string): void {
    if (id) {
      this.streamService.getStream(id, this.token).subscribe(stream => {
        this.selectedStream = stream;
        this.main.convName = stream.name;
        this.main.convEmoji = stream.emoji;
        this.main.convImage = this.getIcon(stream);
        this.viewOnly = stream.viewOnly;
        var flags = new StreamFlags(stream.streambits);
        this.gridView = flags.gridView;
        this.anonymous = flags.anonymous;
        this.onResize(null);
      });
    }
  }

  isSelected(stream: Stream): boolean {
    return this.selectedStream && stream._id == this.selectedStream._id;
  }

  private hasPublicRedirect(): boolean {
    return this.me && this.me.name == "public" && (this.site && this.site.publicStreamURL && !this.site.publicShowMarket);
  }

  private shouldShowAtAll(): boolean {
    return this.me && this.flags && !(this.me.name == "public" &&
      this.flags.hideStreamsWhenPublic) &&
      this.router.url.indexOf("/timeline") < 0 &&
      this.router.url.indexOf("/map") < 0;
  }

  private shouldShowAtStart(): boolean {
    return this.me && (this.gridView || this.isSmall());
  }

  selectStream(stream: Stream) {
    this.selectedStream = stream;
    this.unread -= stream.unread;
    this.setTitle();
    stream.unread = 0;
    this.resetStreams();
    if (!this.shouldShowAtStart()) {
      if (!this.viewOnly) {
        this.showActionsService.show();
      }
    }
    this.router.navigateByUrl(`/streams/${stream._id}`);
    this.streamOpenService.select(this.selectedStream._id);
  }

  private resetStreams() {
    var oldstreams = [];
    this.streams.forEach(s => oldstreams.push(s));
    this.streams = oldstreams;
  }

  tabTitle(): string {
    return this.site && this.site.tabTitle ? this.site.tabTitle : "Conversations";
  }

  headerTitle(): string {
    return this.site && this.site.headerTitle ? this.site.headerTitle : "Visual Ops";
  }

  private setTitle(): void {
    document.title = (this.unread > 0 ? "(" + this.unread + ") " : "") + this.headerTitle();
  }

  private showStreams(streams: Stream[]): void {
    if (!this.me) {
      return;
    }
    if (this.me.name == "public") {
      this.streamService.getSelectedStreams(streams.map(e => e._id)).subscribe(streams => {
        if (streams && streams.length > 0) {
          this.loadStreams(streams);
          this.loadSockets(streams.map(e => e._id));
        }
        else if (!this.hasPublicRedirect()) {
          this.gotoMarket();
        }
      });
    }
    else {
      this.streamService.getStreams(this.maxStreams, true).subscribe(allstreams => {

        var showstreams = [];
        allstreams.forEach(e => {
          if (e.unread > 0) {
            showstreams.push(e);
          }
        });
        var missing = [];
        streams.forEach(e => {
          var s = allstreams.filter(e2 => e2 && e && e2._id == e._id);
          if (s.length > 0) {
            if (showstreams.filter(e2 => e2._id == e._id).length == 0) {
              showstreams.push(s[0]);
            }
          }
          else {
            if (e && e._id) {
              missing.push(e._id);
            }
          }
        });
        if (missing.length > 0) {
          this.streamService.getSelectedStreams(missing).subscribe(streams => {
            if (streams && streams.length > 0) {
              // still missing means that we can't see it anymore. Probably
              // security, just ignore it.
              streams.forEach(s => showstreams.push(s));
            }
            this.finishGetStreams(showstreams, allstreams);
          });
        }
        else {
          this.finishGetStreams(showstreams, allstreams);
        }

      });
    }
  }

  private getMinStreamsForMarket(): number {
    return this.site && this.site.minStreamsForMarket ? this.site.minStreamsForMarket : this.minStreamsForMarket;
  }

  private getStreams(): void {

    if (!this.me) {
      return;
    }

    if (this.me.name == "public") {
      this.streamService.getStreams(this.maxStreams, false).subscribe(streams => {
        this.loadStreams(streams);
        this.loadSockets(streams.map(e => e._id));
      });
    }
    else {
      this.settingsService.getSettings(this.me, this.site.domain ? this.site.domain : "convo").subscribe(settings => {

        if (settings) {
          if (!settings.streams || settings.streams.length == 0) {
            let min = this.getMinStreamsForMarket();
            this.streamService.getStreams(min+1, true).subscribe(streams => {
              if (streams.length <= min) {
                this.loadStreams(streams);
                this.loadSockets(streams.map(e => e._id));
              }
              else {
                // if they are navigating directly to an idea, don't show the market.
                if (this.router.url.indexOf("/ideas/") < 0 && this.router.url.indexOf("/streams/") < 0) {
                  this.gotoMarket();
                }
              }
            });
          }
          else {
            this.showStreams(settings.streams);
          }
        }

      });
    }

  }

  private finishGetStreams(showstreams: Stream[], allstreams: Stream[]) {
    this.loadStreams(showstreams);
    var ids = showstreams.map(e => e._id);
    allstreams.forEach(s => ids.push(s._id));
    this.loadSockets(_.uniq(ids));
  }

  private loadSockets(ids: string[]) {
    if (this.sockets && ids.length > 0) {
      ids.push("streams");
      this.socketService.setStreams(ids);
      this.socketService.openDocs();
    }
  }

  private loadStreams(streams: Stream[]) {
    this.unread = 0;
    _.forEach(streams, e => {
      this.unread += e.unread;
    });
    this.setTitle();
    this.streams = streams;
    // mark the stream that is the room.
    let room = this.mediarouterService.getRoom();
    this.streams.forEach(e => {
      if (e._id == room) {
        e.room = true;
      }
    });
  }

  addStreamDialog(): void {
    if (this.site.newStreamSequence && this.site.newStreamIdeas) {
      this.router.navigateByUrl(`/newidea/${this.site.newStreamIdeas}/${this.site.newStreamSequence}?createidea=true&returnblank=true&hidemenu=true`);
    }
    else {
      this.dialog.open(AddStreamDialogComponent, {}).afterClosed().subscribe(result => {
        if (result) {
          this.streamService.addStream({
              name: result.name,
              users: result.users,
              streambits: result.newusers ? 2048 : 0
            } as Stream).subscribe((stream) => {
            if (stream._id) {
              this.streamWaiting = null;
              this.selectStream(stream);
            }
            else {
              this.streamWaiting = result.name;
            }
          });
        }
      });
    }
  }

  getIcon(item: Stream): string {
    return this.iconService.getIcon(item);
  }

  onDrag(event: any) {
    console.log(event);
  }

  private updateStreamSettingsWithStreams(streams: Stream[]): Observable<any> {
    return this.settingsService.updateSettings(this.me, this.site.domain ? this.site.domain : "convo", {
      streams: streams
    });
  }

  private isPublic(): boolean {
    return this.me && this.me.name == "public";
  }

  private peopleShouldShow(): boolean {
    return !this.isPublic() && !this.anonymous;
  }

  private hasLanding(): boolean {
    return this.site && this.site.landingSequence && this.site.landingSequence.length > 0 && (this.isPublic() || this.site.allSeeLandingSequence);
  }

  onResize(event: any) {

    if (this.shouldShowAtAll()) {
      if (this.hasLanding()) {
        this.opened = false;
      }
      else {
        // never open in the market place.
        this.opened = this.router.url.indexOf("/market") < 0 && this.router.url.indexOf("/seqmarket") < 0;
        if (this.shouldShowAtStart()) {
          this.opened = this.opened && this.selectedStream == null;
        }
      }
    }
    else {
      this.opened = false;
    }

    let fullheight = window.innerHeight-68;
    if (!this.viewOnly && this.selectedStream) {
      let height = this.peopleShouldShow() ? (fullheight / 2) - 8 : fullheight;
      this.listheight = (height - 24) + "px";
    }
    else {
      this.listheight = (fullheight - 42) + "px";
    }
  }

  private isSmall(): boolean {
    return this.breakpointObserver.isMatched("(max-width: 960px)");
  }

  close() {
    this.opened = false;
  }

  gotoMarket() {
    this.main.newstreams = 0;
    if (this.selectedStream) {
      this.router.navigateByUrl("/market/" + this.selectedStream._id);
    }
    else {
      this.router.navigateByUrl("/market");
    }
  }

  private hasAdmin(): boolean {
    return this.me && this.me.admin;
  }

  showNewStream(): boolean {
    return this.hasAdmin() || (this.flags && this.flags.showNewStream);
  }
}
