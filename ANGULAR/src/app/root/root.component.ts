import { Component, OnInit } from '@angular/core';
import { WebsocketWrapperService } from '../websocket-wrapper.service';
import { WebSocketModule, Commands_t } from '../websocket-module';

@Component({
  selector: 'app-root',
  templateUrl: './root.component.html',
  styleUrls: ['./root.component.scss']
})

export class RootComponent implements OnInit {
  public websocket: WebsocketWrapperService;
  private requestMade: boolean = false;

  constructor(private ws: WebsocketWrapperService) {
    // this.modules = _modules;
    this.websocket = ws;
    var self = this;
    this.websocket.moduleAdded.subscribe(
      $event => {
        self._OnAddedModule($event);
      });
  }

  ngOnInit() {
    this.websocket.modules.forEach(element => {
      this._Request(element);
    });
  }

  private _OnAddedModule($event): void {
    this._Request($event);
  }

  private _Request(mod: WebSocketModule): void {
    if (!this.requestMade) {
      mod.Request(Commands_t.relay);
      this.requestMade = true;
    }
  }
}
