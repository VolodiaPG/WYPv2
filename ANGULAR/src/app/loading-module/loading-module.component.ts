import { Component, Output, EventEmitter, Input } from '@angular/core';

@Component({
  selector: 'app-loading-module',
  templateUrl: './loading-module.component.html',
  styleUrls: ['./loading-module.component.scss']
})
export class LoadingModuleComponent {
  private _visible = true;
  private _previousState = true;

  @Output() hiddenEvent: EventEmitter<any> = new EventEmitter();
  @Output() visibleEvent: EventEmitter<any> = new EventEmitter();

  constructor() { }

  get visible(): boolean {
    return this._visible;
  }

  @Input() set visible(value: boolean) {
    this._visible = value;
    if (value && !this._previousState) {
      this.visibleEvent.emit();
      console.log('value', value);
    } else if (!value && this._previousState) {
      this.hiddenEvent.emit();
      console.log('value', value);
    }
    this._previousState = value;
  }

}
