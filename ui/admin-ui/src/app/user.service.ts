import { Injectable } from '@angular/core';
import { Observable ,  of } from 'rxjs';
import { HttpClient, HttpHeaders, HttpResponse } from '@angular/common/http';
import { catchError, map, tap } from 'rxjs/operators';
import { MatDialog, MatDialogRef } from '@angular/material/dialog';

import { BackendService } from './backend.service';
import { User } from './user';
import { SocketService }  from './socket.service';
import { UpService }  from './up.service';

@Injectable({
  providedIn: 'root'
})
export class UserService extends BackendService {

  private usersUrl = '/rest/1.0/users';

  constructor(
    dialog: MatDialog,
    socketService: SocketService,
    upService: UpService,
    http: HttpClient,
  ) {
    super(dialog, socketService, upService, http)
  }

  getUsers(offset: number, limit: number): Observable<HttpResponse<User[]>> {
    const url = `${this.usersUrl}?offset=${offset}&limit=${limit}`;
    return this.http.get<User[]>(url, { observe: 'response' })
    .pipe(
     catchError(this.handleResponseError<User[]>('getUsers', []))
    );
  }

}
