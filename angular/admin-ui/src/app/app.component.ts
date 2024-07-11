import { Component, ViewChild } from '@angular/core';
import { MatSidenav } from '@angular/material/sidenav';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})
export class AppComponent {

  @ViewChild('sidenav') public sidenav: MatSidenav;

  title = 'admin-ui';
  height = "300px";
  opened = true;

  onResize() {
    this.height = window.innerHeight + "px";
  }

  headerTitle(): string {
    return "ZMQChat";
  }

  version(): string {
    return "0.1";
  }

  logoff() {
    window.open("/logout", "_self");
  }

  close(): void {
    this.opened = false;
    this.sidenav.close();
  }
  open(): void {
    this.opened = true;
    this.sidenav.open();
  }

}
